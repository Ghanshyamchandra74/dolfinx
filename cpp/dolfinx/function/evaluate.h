// Copyright (C) 2020 Jack S. Hale
//
// This file is part of DOLFINX (https://www.fenicsproject.org)
//
// SPDX-License-Identifier:    LGPL-3.0-or-later
//

#pragma once

#include <vector>

#include "Expression.h"
#include "utils.h"
#include <Eigen/Dense>
#include <dolfinx/fem/FormCoefficients.h>
#include <dolfinx/mesh/Mesh.h>

namespace dolfinx::function
{

template <typename T>
class Expression;

/// Evaluate a UFC expression.
/// @param[in,out] values An array to evaluate the expression into
/// @param[in] e The expression to evaluate
/// @param[in] active_cells The cells on which to evaluate the expression
template <typename T>
void eval(Eigen::Ref<Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> values,
          const function::Expression<T>& e,
          const std::vector<std::int32_t>& active_cells);

template <typename T>
void eval(Eigen::Ref<Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> values,
          const function::Expression<T>& e,
          const std::vector<std::int32_t>& active_cells)
{
  // Extract data from Expression
  auto mesh = e.mesh();
  assert(mesh);

  // Prepare coefficients
  Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> coeffs
      = pack_coefficients(e);

  // Prepare constants
  if (!e.all_constants_set())
    throw std::runtime_error("Unset constant in Form");
  const Eigen::Array<T, Eigen::Dynamic, 1> constant_values = pack_constants(e);

  const auto& fn = e.get_tabulate_expression();

  // Prepare cell geometry
  const graph::AdjacencyList<std::int32_t>& x_dofmap = mesh->geometry().dofmap();

  // FIXME: Add proper interface for num coordinate dofs
  const int num_dofs_g = x_dofmap.num_links(0);
  const Eigen::Array<double, Eigen::Dynamic, 3, Eigen::RowMajor>& x_g
      = mesh->geometry().x();

  // Create data structures used in evaluation
  const int gdim = mesh->geometry().dim();
  Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      coordinate_dofs(num_dofs_g, gdim);

  Eigen::Array<T, Eigen::Dynamic, 1> values_e;
  const Eigen::Index num_points = e.num_points();
  const Eigen::Index value_size = e.value_size();
  const Eigen::Index size = num_points * value_size;
  values_e.setZero(size);

  // Iterate over cells and 'assemble' into values
  for (std::size_t i = 0; i < active_cells.size(); ++i)
  {
    std::int32_t c = active_cells[i];

    auto x_dofs = x_dofmap.links(c);
    for (int j = 0; j < num_dofs_g; ++j)
      coordinate_dofs.row(j) = x_g.row(x_dofs[j]).head(gdim);

    auto coeff_cell = coeffs.row(c);

    values_e.setZero();
    fn(values_e.data(), coeff_cell.data(), constant_values.data(),
       coordinate_dofs.data());

    for (Eigen::Index k = 0; k < size; ++k)
      values(i, k) = values_e[k];
  }
}

} // namespace dolfinx::function
