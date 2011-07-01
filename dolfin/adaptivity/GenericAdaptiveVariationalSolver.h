// Copyright (C) 2010 Marie E. Rognes
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Anders Logg, 2010-2011.
//
// First added:  2010-08-19
// Last changed: 2011-03-31

#ifndef __GENERIC_ADAPTIVE_VARIATIONAL_SOLVER_H
#define __GENERIC_ADAPTIVE_VARIATIONAL_SOLVER_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <dolfin/common/Variable.h>
#include <dolfin/fem/BoundaryCondition.h>
#include <dolfin/adaptivity/AdaptiveDatum.h>

namespace dolfin
{
  // Forward declarations
  class Form;
  class Function;
  class FunctionSpace;
  class ErrorControl;
  class GoalFunctional;
  class Parameters;

  /// An abstract class for goal-oriented adaptive solution of
  /// variational problems.

  class GenericAdaptiveVariationalSolver : public Variable
  {
  public:

    /// Solve such that the error measured in the functional 'goal' is
    /// less than the given tolerance using the ErrorControl object
    /// 'control'
    ///
    /// *Arguments*
    ///
    ///     tol  (double)
    ///         The error tolerance
    ///
    ///     goal  (_Form_)
    ///         The goal functional
    ///
    ///     control  (_ErrorControl_)
    ///         The error controller
    ///
    void solve(const double tol, Form& goal, ErrorControl& control);

    /// Solve such that the error measured in the goal functional 'M'
    /// is less than the given tolerance using the GoalFunctional's
    /// ErrorControl object. Must be overloaded in subclass.
    ///
    /// *Arguments*
    ///
    ///     tol  (double)
    ///         The error tolerance
    ///
    ///     goal  (_GoalFunctional_)
    ///         The goal functional
    ///
    virtual void solve(const double tol, GoalFunctional& M) = 0;

    /// Solve the primal problem. Must be overloaded in subclass.
    ///
    /// *Returns*
    ///
    ///     boost::shared_ptr<const _Function_>
    ///         The solution to the primal problem
    ///
    virtual boost::shared_ptr<const Function> solve_primal() = 0;

    /// Extract the boundary conditions for the primal problem. Must
    /// be overloaded in subclass.
    ///
    /// *Returns*
    ///
    ///     std::vector<boost::shared_ptr<const BoundaryCondition> >
    ///         The primal boundary conditions
    ///
    virtual std::vector<boost::shared_ptr<const BoundaryCondition> >
    extract_bcs() const = 0;

    /// Evaluate the goal functional. Must be overloaded in subclass.
    ///
    /// *Arguments*
    ///
    ///    M (_Form_)
    ///        The functional to be evaluated
    ///
    ///    u (_Function_)
    ///        The function of which to evaluate the functional
    ///
    /// *Returns*
    ///
    ///     double
    ///         The value of M evaluated at u
    ///
    virtual double evaluate_goal(Form& M,
                                 boost::shared_ptr<const Function> u) const = 0;

    /// Adapt the problem to other mesh. Must be overloaded in subclass.
    ///
    /// *Arguments*
    ///
    ///    mesh (_Mesh_)
    ///        The other mesh
    ///
    virtual void adapt_problem(boost::shared_ptr<const Mesh> mesh) = 0;

    /// Default parameter values:
    ///
    ///     "max_iterations" (int)
    ///     "max_dimension"  (int)
    ///     "plot_mesh"  (bool)
    ///     "reference"  (double)
    ///     "marking_strategy"  (string)
    ///     "marking_fraction"  (double)
    ///
    static Parameters default_parameters()
    {
      Parameters p("adaptive_solver");

      // Set default generic adaptive parameters
      p.add("max_iterations", 20);
      p.add("max_dimension", 0);
      p.add("plot_mesh", false); // Useful for debugging
      p.add("reference", 0.0);
      p.add("marking_strategy", "dorfler");
      p.add("marking_fraction", 0.5, 0.0, 1.0);

      return p;
    }

  protected:

    /// Check if stopping criterion is satisfied
    bool stop(const FunctionSpace& V,
              const double error_estimate,
              const double tolerance,
              const Parameters& parameters);

    /// Present summary of all adaptive data
    void summary(const std::vector<AdaptiveDatum>& data,
                 const Parameters& parameters);

    /// Present summary of single adaptive datum
    void summary(const AdaptiveDatum& data);

  };

}



#endif