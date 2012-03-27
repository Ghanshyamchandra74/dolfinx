// Copyright (C) 2010 Anders Logg
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
// First added:  2010-10-19
// Last changed: 2010-10-19

#include <dolfin/common/MPI.h>
#include <dolfin/mesh/MeshPartitioning.h>
#include <dolfin/mesh/MeshEditor.h>
#include "UnitTetrahedron.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
UnitTetrahedron::UnitTetrahedron() : Mesh()
{
  // Receive mesh according to parallel policy
  if (MPI::is_receiver()) { MeshPartitioning::build_distributed_mesh(*this); return; }

  // Open mesh for editing
  MeshEditor editor;
  editor.open(*this, CellType::tetrahedron, 3, 3);

  // Create vertices
  editor.init_vertices(4);
  editor.add_vertex(0, 0, 0, 0);
  editor.add_vertex(1, 1, 0, 0);
  editor.add_vertex(2, 0, 1, 0);
  editor.add_vertex(3, 0, 0, 1);

  // Create cells
  editor.init_cells(1);
  editor.add_cell(0, 0, 1, 2, 3);

  // Close mesh editor
  editor.close();

  // Broadcast mesh according to parallel policy
  if (MPI::is_broadcaster()) { MeshPartitioning::build_distributed_mesh(*this); return; }
}
//-----------------------------------------------------------------------------