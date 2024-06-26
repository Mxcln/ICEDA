#include <Main/HLSHandler.h>
#include <Main/RTLGener.h>
#include <iostream>
#include <unordered_map>
#include <queue>

namespace hls {

HLSHandler::HLSHandler()
{
  _func = new function();
  _parser = new parser::parser(_func);
  _rtl_gener = new RTLGener(_func);
}

HLSHandler::~HLSHandler()
{
  delete _func;
  delete _parser;
  delete _rtl_gener;
}

void HLSHandler::run(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return;
  }
  _parser->set_filepath(argv[1]);
  if (_parser->parse() != 0)
  {
    std::cerr << "Error parsing file: " << argv[1] << std::endl;
    return;
  }

  schedule();
  _rtl_gener->generate("top.v");
  
}

void HLSHandler::global_binding(){
  std::vector<basic_block> bbs = _func->get_basic_blocks();
  std::vector<std::string> var_names_0 = bbs[0].get_var_names();
  std::vector<std::string> var_names_1 = bbs[1].get_var_names();
  std::vector<std::string> var_names_2 = bbs[2].get_var_names();
  std::vector<std::string> var_names_3 = bbs[3].get_var_names();

  std::vector<std::string> global_regs;
  std::set_intersection(var_names_0.begin(), var_names_0.end(), var_names_1.begin(), var_names_1.end(), std::back_inserter(global_regs));
  std::set_intersection(var_names_1.begin(), var_names_1.end(), var_names_2.begin(), var_names_2.end(), std::back_inserter(global_regs));

  for(size_t i=0;i<global_regs.size();i++)
  {
    _func->add_global_reg(global_regs[i]);
  }
}

void HLSHandler::binding(basic_block& bb,int& colors){
  std::cout<<"binding block:"<<bb.get_name()<<std::endl;
  std::vector<statement>& ss = bb.get_statements();
  std::queue<point*> q;
  std::queue<int> free_color;
  free_color.push(colors);

  for(size_t i=0;i<ss.size();i++)
  { 
    statement& s = ss[i];
    std::string v = s.get_var();
    if (v.empty())
    {
      continue;
    }
    
    bb.points[v] = new point(v);
    for (int j=0;j<s.get_num_oprands();j++)
    {
      if(s.get_type() == OP_TYPE::OP_LOAD)
        {
          if(j==0)
          {
            _func->add_reg_map(s.get_oprand(j),-3);
            continue;
          }
        }

      if(s.get_type() == OP_TYPE::OP_PHI)
      {
        if((j==1)||(j==3))
        { 
          _func->add_reg_map(s.get_oprand(j),-2);
          continue; 
        }
      }

      if(s.get_type() == OP_TYPE::OP_BR)
      {
        continue;
      }

      std::string name = s.get_oprand(j);
      
      auto it = bb.points.find(name);
      if (it == bb.points.end())   
      {

        if(name == "0" || name == "1")
          continue;

        if(name == "n")
        {
          _func->add_reg_map(name,-4);
          continue;
        }

        bb.add_input(name);
        bb.points[name] = new point(name);
      }
      bb.points[name]->add_next_point(v);
      bb.points[v]->add_prev_point(name);
      bb.points[v]->in_num++;
    }
  }

  for (const auto& p : bb.points)
  {
    if (p.second->get_next_points().empty())
    {
      bb.add_output(p.second->get_value());
    }
  }

  for(const auto& p : bb.points)
  {
    if (p.second->in_num == 0)
    {
      q.push(p.second);
      p.second->set_begin_cycle(0);

      if(_func->is_global_reg(p.second->get_value())>=0)
      {
        _func->add_reg_map(p.second->get_value(),_func->is_global_reg(p.second->get_value()));
      }

      else if(free_color.empty())
      {
        colors++;
        _func->add_reg_map(p.second->get_value(),colors);
      }

      else
      {
        _func->add_reg_map(p.second->get_value(),free_color.front());
        free_color.pop();
      }
    }
  }

  while(!q.empty())
  {
    point* p = q.front();
    q.pop();       
    size_t k = 0;
    while(k<p->get_next_points().size())
    {
      std::string n = p->get_next_points()[k];
      bb.points[n]->in_num--;
      if (bb.points[n]->in_num == 0)
      { 
        if(p->get_value()=="ai" || p->get_value()=="bi")
        {
          bb.points[n]->set_begin_cycle(p->get_begin_cycle() + 2);
        }
        else
        {
          bb.points[n]->set_begin_cycle(p->get_begin_cycle() + 1);
        }
        for (const auto& m : bb.points[n]->get_prev_points())
        {
          bb.points[m]->delete_next_point(bb.points[n]->get_value());
          if (bb.points[m]->get_next_points().empty())
          {
            bb.points[m]->set_end_cycle(bb.points[n]->get_begin_cycle());
            auto it = std::find(bb.get_outputs().begin(), bb.get_outputs().end(), bb.points[m]->get_value());
            if (it == bb.get_outputs().end())   //如果不是输出节点，生命周期结束
            {
              free_color.push(bb.points[m]->get_color());
            }
          }
        }

        if(_func->is_global_reg(bb.points[n]->get_value())>=0)
        {
          _func->add_reg_map(n,_func->is_global_reg(bb.points[n]->get_value()));
        }
        else if(free_color.empty())
        {
          colors++;
          _func->add_reg_map(n,colors);
        }
        else
        {
          _func->add_reg_map(n,colors);
          free_color.pop();
        }

        q.push(bb.points[n]);
      }
      else{
        k++;
      }
    }
  }

  int end_cycle = 0;            //block的结束周期
  for (const auto& p : bb.get_outputs())
  {
    end_cycle = std::max(end_cycle, bb.points[p]->get_begin_cycle()+1);
  }
  bb.set_end_cycle(end_cycle);


  bb.set_colors(colors);
  std::cout << "block:" << bb.get_name()  << " end_cycle:" << bb.get_end_cycle() << std::endl;

  //bindding to statement
  for(size_t i=0;i<ss.size();i++)
  {
    statement& s = ss[i];
    std::string v = s.get_var();
    if (v.empty())
    {
      if(bb.get_end_cycle()>0)
        s.set_begin_cycle(std::max(0,bb.get_end_cycle()-1));
      else
      {
        bb.set_end_cycle(1);
        s.set_begin_cycle(0);
      }
      continue;
    }
    if (s.get_type() == OP_TYPE::OP_LOAD)
    {
      s.set_begin_cycle(std::max(0,bb.points[v]->get_begin_cycle()-2));
    }
    else
    {
      s.set_begin_cycle(std::max(0,bb.points[v]->get_begin_cycle()-1));
    }

    // bb.add_statement_by_cycle(&s,s.get_begin_cycle());

    std::cout<< s.get_begin_cycle() << " "  << s.get_var() << "(" << _func->get_reg_map(s.get_var()) << ")"  << " ";
    for(int j=0;j<s.get_num_oprands();j++)
    {
      std::cout << s.get_oprand(j) << "(" << _func->get_reg_map(s.get_oprand(j)) << ") ";
    }
    std::cout<<std::endl;
  }

}

void HLSHandler::schedule()
{
  std::vector<basic_block>& bbs = _func->get_basic_blocks();
  int color=0;
  color = color + _func->get_global_regs().size();
  for(size_t i=0;i<bbs.size();i++)
  {
     binding(bbs[i],color);
  }

  for (auto& block : _func->get_basic_blocks()) {
    auto& hash = block.get_statements_by_cycle();
    for (auto& statement : block.get_statements()) {
      hash[statement.get_begin_cycle()].push_back(&statement);
    }
  }

}
} // namespace hls
