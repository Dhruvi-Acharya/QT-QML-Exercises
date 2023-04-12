#pragma once

#include <functional>

// reference site : https://gist.github.com/tsaarni/bb0b8d1ca33e3a1bfea1

template <typename Guard, typename Function>
struct weak_callback_details
{
   std::weak_ptr<typename Guard::element_type> g_;
   Function f_;

   weak_callback_details(Guard&& g, Function&& f)
      : g_(std::forward<Guard>(g)), f_(std::forward<Function>(f)) { }

   template <typename ...Args>
   void operator() (Args&& ...args)
   {
      // weak_ptr is used as guard:
      //  - if weak_ptr is not expired, call provided function
      //  - if weak_ptr is expired, skip the call
      if (auto s = g_.lock())
      {
         //std::cout << "guard object alive: executing function" << std::endl;
         f_(std::forward<Args>(args)...);
      }
      else
      {
         //std::cout << "guard object released: do not execute function" << std::endl;
      }
   }
};

template <typename Guard, typename Function>
weak_callback_details<Guard, Function> WeakCallback(Guard&& g, Function&& f)
{
   return weak_callback_details<Guard, Function>(std::forward<Guard>(g), std::forward<Function>(f));
}
