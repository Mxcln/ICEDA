#pragma once

#include <algorithm>
#include <mockturtle/algorithms/aig_balancing.hpp>
#include <mockturtle/algorithms/balancing.hpp>
#include <mockturtle/algorithms/balancing/sop_balancing.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <vector>
#include <mockturtle/algorithms/cut_enumeration.hpp>


using namespace percy;
using namespace mockturtle;

namespace phyLS
{
template <class Ntk>
void aig_balancing(Ntk& ntk) {
    sop_rebalancing<Ntk> balance_fn;
    balancing_params ps;
    ps.cut_enumeration_ps.cut_size = 4u;
    const auto balanced_aig = balancing( ntk, {balance_fn}, ps );
    ntk = balanced_aig;
};
}
