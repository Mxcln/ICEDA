#pragma once

#include <mockturtle/algorithms/cut_rewriting.hpp>
#include <mockturtle/algorithms/node_resynthesis.hpp>
#include <mockturtle/algorithms/node_resynthesis/akers.hpp>
#include <mockturtle/algorithms/node_resynthesis/exact.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/properties/mccost.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/cost_functions.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include "mockturtle/algorithms/cleanup.hpp"
#include "mockturtle/algorithms/cut_enumeration.hpp"
#include "mockturtle/algorithms/detail/mffc_utils.hpp"
// #include "mockturtle/algorithms/dont_cares.hpp"
// #include "mockturtle/networks/klut.hpp"
// #include "mockturtle/networks/mig.hpp"
#include "mockturtle/traits.hpp"
#include "mockturtle/utils/cost_functions.hpp"
#include "mockturtle/utils/node_map.hpp"
#include "mockturtle/utils/progress_bar.hpp"
// #include "mockturtle/views/cut_view.hpp"
// #include "mockturtle/views/depth_view.hpp"
// #include "mockturtle/views/fanout_view.hpp"

#include <fmt/format.h>
#include <kitty/print.hpp>

using namespace percy;
using namespace mockturtle;

namespace phyLS {
struct cut_enumeration_cut_rewriting_cut {
  int32_t gain{-1};
};

// using mffc_size = mockturtle::detail::mffc_size;
// using cut_rewriting_params = mockturtle::cut_rewriting_params;
// using signal = mockturtle::signal;
// template<class Ntk>
// using mockturtle::unit_cost<Ntk> = mockturtle::unit_cost<Ntk>;

template <class Ntk> void aig_rewrite(Ntk &ntk) {
  // please learn the algorithm of "mockturtle/algorithms/cut_rewriting.hpp"

  xag_npn_resynthesis<Ntk> resync;

  cut_rewriting_params ps;
  ps.cut_enumeration_ps.cut_size = 4;
  ps.allow_zero_gain = true;

  node_map<mockturtle::signal<Ntk>, Ntk> old2new(ntk);
  Ntk res;
  old2new[ntk.get_constant(false)] = res.get_constant(false);
  if (ntk.get_node(ntk.get_constant(true)) !=
      ntk.get_node(ntk.get_constant(false))) {
    old2new[ntk.get_constant(true)] = res.get_constant(true);
  }
  ntk.foreach_pi([&](auto const &n) { old2new[n] = res.create_pi(); });

  const auto cuts =
      cut_enumeration<Ntk, true, cut_enumeration_cut_rewriting_cut>(
          ntk, ps.cut_enumeration_ps);

  mockturtle::detail::initialize_values_with_fanout(ntk);

  const auto orig_cost = costs<Ntk, mockturtle::unit_cost<Ntk>>(ntk);

  ntk.foreach_gate([&](auto const &n, auto i) {
    /* nothing to optimize? */
    int32_t value =
        mockturtle::detail::mffc_size<Ntk, mockturtle::unit_cost<Ntk>>(ntk, n);
    if (value == 1) {
      std::vector<mockturtle::signal<Ntk>> children(ntk.fanin_size(n));
      ntk.foreach_fanin(n, [&](auto const &f, auto i) {
        children[i] =
            ntk.is_complemented(f) ? res.create_not(old2new[f]) : old2new[f];
      });

      old2new[n] = res.clone_node(ntk, n, children);
    } else {
      /* foreach cut */
      int32_t best_gain = -1;
      mockturtle::signal<Ntk> best_signal;
      for (auto &cut : cuts.cuts(ntk.node_to_index(n))) {
        /* skip small enough cuts */
        if (cut->size() == 1 || cut->size() < ps.min_cand_cut_size)
          continue;

        const auto tt = cuts.truth_table(*cut);
        assert(cut->size() == static_cast<unsigned>(tt.num_vars()));

        std::vector<mockturtle::signal<Ntk>> children(cut->size());
        auto ctr = 0u;
        for (auto l : *cut) {
          children[ctr++] = old2new[ntk.index_to_node(l)];
        }

        const auto on_signal = [&](auto const &f_new) {
          auto value2 = mockturtle::detail::recursive_ref<
              Ntk, mockturtle::unit_cost<Ntk>>(res, res.get_node(f_new));
          mockturtle::detail::recursive_deref<Ntk, mockturtle::unit_cost<Ntk>>(
              res, res.get_node(f_new));
          int32_t gain = value - value2;

          if ((gain > 0 || (ps.allow_zero_gain && gain == 0)) &&
              gain > best_gain) {
            if constexpr (has_level_v<Ntk>) {
              if (!ps.preserve_depth ||
                  res.level(res.get_node(f_new)) <= ntk.level(n)) {
                best_gain = gain;
                best_signal = f_new;
              }
            } else {
              best_gain = gain;
              best_signal = f_new;
            }
          }

          return true;
        };
        resync(res, cuts.truth_table(*cut), children.begin(), children.end(),
               on_signal);
      }

      if (best_gain == -1) {
        std::vector<mockturtle::signal<Ntk>> children(ntk.fanin_size(n));
        ntk.foreach_fanin(n, [&](auto const &f, auto i) {
          children[i] =
              ntk.is_complemented(f) ? res.create_not(old2new[f]) : old2new[f];
        });

        old2new[n] = res.clone_node(ntk, n, children);
      } else {
        old2new[n] = best_signal;
      }
    }

    mockturtle::detail::recursive_ref<Ntk, mockturtle::unit_cost<Ntk>>(
        res, res.get_node(old2new[n]));
  });

  /* create POs */
  ntk.foreach_po([&](auto const &f) {
    res.create_po(ntk.is_complemented(f) ? res.create_not(old2new[f])
                                         : old2new[f]);
  });

  Ntk ret = cleanup_dangling<Ntk>(res);

  /* new costs */
  ntk = costs<Ntk, mockturtle::unit_cost<Ntk>>(ret) > orig_cost
            ? static_cast<Ntk>(ntk)
            : ret;

  ntk = cleanup_dangling(ntk);
}
} // namespace phyLS