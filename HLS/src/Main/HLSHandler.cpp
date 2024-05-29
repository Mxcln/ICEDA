#include <Main/HLSHandler.h>
#include <iostream>
#include <unordered_map>
#include <queue>

namespace hls {

HLSHandler::HLSHandler()
{
  _func = new function();
  _parser = new parser::parser(_func);
}

HLSHandler::~HLSHandler()
{
  delete _func;
  delete _parser;
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

  std::cout << *_func;
  schedule();
}

void HLSHandler::binding(basic_block& bb){
  std::cout<<"binding block:"<<bb.get_name()<<std::endl;
  std::vector<statement> ss = bb.get_statements();
  std::unordered_map<std::string, point*> points;
  std::queue<point*> q;
  std::queue<int> free_color;
  int colors=0;
  free_color.push(colors);

  for(size_t i=0;i<ss.size();i++)
  { 
    statement s = ss[i];
    std::string v = s.get_var();
    if (v.empty())
    {
      continue;
    }
    points[v] = new point(v);
    for (int j=0;j<s.get_num_oprands();j++)
    {
      if(s.get_type() == OP_TYPE::OP_LOAD)
        {
          if(j==0)
          {
            // std::cout<<" load op "<<j<<std::endl;
            continue;
          }
        }

      if(s.get_type() == OP_TYPE::OP_PHI)
      {
        if((j==1)||(j==3))
        { 
          continue;
        }
      }

      if(s.get_type() == OP_TYPE::OP_BR)
      {
        continue;
      }

      std::string name = s.get_oprand(j);
      auto it = points.find(name);
      if (it == points.end())   
      {
        if(name == "0" || name == "1")
          continue;
        bb.add_input(name);
        points[name] = new point(name);
      }
      points[name]->add_next_point(v);
      points[v]->add_prev_point(name);
      points[v]->in_num++;
    }
  }

  for (const auto& p : points)
  {
    if (p.second->get_next_points().empty())
    {
      bb.add_output(p.second->get_value());
    }
  }

  for(const auto& p : points)
  {
    if (p.second->in_num == 0)
    {
      q.push(p.second);
      p.second->set_begin_cycle(0);
      if(free_color.empty())
      {
        colors++;
        p.second->set_color(colors);
      }
      else
      {
        p.second->set_color(free_color.front());
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
      points[n]->in_num--;
      if (points[n]->in_num == 0)
      { 
        points[n]->set_begin_cycle(p->get_begin_cycle() + 1);
        for (const auto& m : points[n]->get_prev_points())
        {
          points[m]->delete_next_point(points[n]->get_value());
          if (points[m]->get_next_points().empty())
          {
            points[m]->set_end_cycle(points[n]->get_begin_cycle());
            auto it = std::find(bb.get_outputs().begin(), bb.get_outputs().end(), points[m]->get_value());
            if (it == bb.get_outputs().end())   //如果不是输出节点，生命周期结束
            {
              free_color.push(points[m]->get_color());
            }
          }
        }

        if(free_color.empty())
        {
          colors++;
          points[n]->set_color(colors);
        }
        else
        {
          points[n]->set_color(free_color.front());
          free_color.pop();
        }

        q.push(points[n]);
      }
      else{
        k++;
      }
    }
  }

  int end_cycle = 0;            //block的结束周期
  for (const auto& p : bb.get_outputs())
  {
    end_cycle = std::max(end_cycle, points[p]->get_begin_cycle()+1);
  }

  bb.set_end_cycle(end_cycle);

  bb.set_colors(colors);

  for (const auto& p : bb.get_outputs())
  {
    points[p]->set_end_cycle(end_cycle);
  }

  // for(const auto& p : points)
  // {
  //   std::cout<<p.second->get_value()<<" "<< "begin:" <<p.second->get_begin_cycle()<<" "<< "end:" <<p.second->get_end_cycle()<<" "<< "color:"<<p.second->get_color()<<std::endl;
  // }


  //bindding to statement
  for(size_t i=0;i<ss.size();i++)
  {
    statement s = ss[i];
    std::string v = s.get_var();
    if (v.empty())
    {
      continue;
    }
    s.set_begin_cycle(std::max(0,points[v]->get_begin_cycle()-1));
    s.add_reg(points[v]->get_color());

    for (int j=0;j<s.get_num_oprands();j++)
    {
      std::string op = s.get_oprand(j);
      auto it = points.find(op);
      if (it != points.end())   
      {
        s.add_reg(points[op]->get_color());
      }
    }

    std::cout<<s.get_var()<<" "<< "begin:" <<s.get_begin_cycle()<<" "<< "regs:";
    for (auto r : s.get_regs())
    {
      std::cout<<r<<" ";
    }
    std::cout<<std::endl;
  }

}

void HLSHandler::schedule()
{
  std::vector<basic_block> bbs = _func->get_basic_blocks();

  for(size_t i=0;i<bbs.size();i++)
  {
     binding(bbs[i]);
  }
}
} // namespace hls
