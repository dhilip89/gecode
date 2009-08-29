/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2009
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <algorithm>

namespace Gecode { namespace Scheduling { namespace Cumulative {

  forceinline void
  OmegaLambdaNode::init(const OmegaLambdaNode& l, const OmegaLambdaNode& r) {
    OmegaNode::init(l,r);
    le = lenv = -Int::Limits::infinity; res = undef;
  }

  forceinline void
  OmegaLambdaNode::update(const OmegaLambdaNode& l, const OmegaLambdaNode& r) {
    OmegaNode::update(l,r);
    le = std::max(l.le + r.e, l.e + r.le);
    if ((r.lenv >= l.env + r.le) && (r.lenv >= l.lenv + r.e)) {
      lenv = r.lenv; res = r.res;
    } else if (l.env + r.le >= l.lenv + r.e) {
      assert(l.env + r.le > r.lenv);
      lenv = l.env + r.le; res = r.res;
    } else {
      assert((l.lenv + r.e > r.lenv) && (l.lenv + r.e > l.env + r.le));
      lenv = l.lenv + r.e; res = l.res;
    }
  }


  template<class TaskView>
  OmegaLambdaTree<TaskView>::OmegaLambdaTree(Region& r, int c0,
                                             const TaskViewArray<TaskView>& t)
    : TaskTree<TaskView,OmegaLambdaNode>(r,t), c(c0) {
    // Enter all tasks into tree (omega = all tasks, lambda = empty)
    for (int i=tasks.size(); i--; ) {
      leaf(i).e = tasks[i].e();
      leaf(i).le = -Int::Limits::infinity;
      leaf(i).env = c*tasks[i].est()+tasks[i].e();
      leaf(i).lenv = -Int::Limits::infinity;
      leaf(i).res = OmegaLambdaNode::undef;
    }
    init();
  }

  template<class TaskView>
  forceinline void 
  OmegaLambdaTree<TaskView>::shift(int i) {
    // i is in omega but not in lambda
    assert((leaf(i).e > 0) && (leaf(i).le == 0));
    leaf(i).le = leaf(i).e;
    leaf(i).e = 0;
    leaf(i).lenv = leaf(i).env;
    leaf(i).env = -Int::Limits::infinity;
    leaf(i).res = i;
    update(i);
  }

  template<class TaskView>
  forceinline void
  OmegaLambdaTree<TaskView>::lremove(int i) {
    // i is lambda but not in omega
    assert((leaf(i).e == 0) && (leaf(i).le > 0));
    leaf(i).le = 0; 
    leaf(i).lenv = -Int::Limits::infinity;
    leaf(i).res = OmegaLambdaNode::undef;
    update(i);
  }

  template<class TaskView>
  forceinline bool
  OmegaLambdaTree<TaskView>::lempty(void) const {
    return root().res < 0;
  }
  
  template<class TaskView>
  forceinline int 
  OmegaLambdaTree<TaskView>::responsible(void) const {
    return root().res;
  }
  
  template<class TaskView>
  forceinline int 
  OmegaLambdaTree<TaskView>::env(void) const {
    return root().env;
  }
  
  template<class TaskView>
  forceinline int 
  OmegaLambdaTree<TaskView>::lenv(void) const {
    return root().lenv;
  }
  
}}}

// STATISTICS: scheduling-prop
