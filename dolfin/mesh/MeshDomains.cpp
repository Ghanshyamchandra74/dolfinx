// Copyright (C) 2011 Anders Logg
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
// Modified by Garth N. Wells, 2012
//
// First added:  2011-08-29
// Last changed: 2011-04-03

#include <limits>
#include <dolfin/common/MPI.h>
#include <dolfin/log/log.h>
#include "MeshFunction.h"
#include "MeshValueCollection.h"
#include "MeshDomains.h"

using namespace dolfin;

const std::size_t MeshDomains::default_unset_value
    = std::numeric_limits<std::size_t>::max();

//-----------------------------------------------------------------------------
MeshDomains::MeshDomains(Mesh& mesh) : _mesh(mesh)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MeshDomains::~MeshDomains()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::size_t MeshDomains::max_dim() const
{
  if (!_markers.empty())
    return _markers.size() - 1;
  else
    return 0;
}
//-----------------------------------------------------------------------------
std::size_t MeshDomains::num_marked(std::size_t dim) const
{
  dolfin_assert(dim < _markers.size());
  return _markers[dim].size();
}
//-----------------------------------------------------------------------------
bool MeshDomains::is_empty() const
{
  std::size_t size = 0;
  for (std::size_t i = 0; i < _markers.size(); i++)
    size += _markers[i].size();
  return size == 0;
}
//-----------------------------------------------------------------------------
std::map<std::size_t, std::size_t>& MeshDomains::markers(std::size_t dim)
{
  dolfin_assert(dim < _markers.size());
  return _markers[dim];
}
//-----------------------------------------------------------------------------
const std::map<std::size_t, std::size_t>&
MeshDomains::markers(std::size_t dim) const
{
  dolfin_assert(dim < _markers.size());
  return _markers[dim];
}
//-----------------------------------------------------------------------------
bool MeshDomains::set_marker(std::pair<std::size_t, std::size_t> marker,
                             std::size_t dim)
{
  dolfin_assert(dim < _markers.size());
  return _markers[dim].insert(marker).second;
}
//-----------------------------------------------------------------------------
std::size_t MeshDomains::get_marker(std::size_t entity_index,
                                    std::size_t dim) const
{
  dolfin_assert(dim < _markers.size());
  std::map<std::size_t, std::size_t>::const_iterator it
    = _markers[dim].find(entity_index);
  if (it == _markers[dim].end())
  {
    dolfin_error("MeshDomains.cpp",
                 "get marker",
                 "Marked entity index does not exist in marked set");
  }

  return it->second;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const MeshFunction<std::size_t> >
  MeshDomains::cell_domains(std::size_t unset_value) const
{
  // Check if any markers have been set
  const std::size_t D = _mesh.topology().dim();
  dolfin_assert(D < _markers.size());

  // Create markers if mesh collection present
  if (!_markers[D].empty() and !_cell_domains)
    _cell_domains = mesh_function(_markers[D], D, unset_value);


  return _cell_domains;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const MeshFunction<std::size_t> >
  MeshDomains::facet_domains(std::size_t unset_value) const
{
  // Check if any markers have been set
  const std::size_t D = _mesh.topology().dim();
  dolfin_assert((D - 1) < _markers.size());

  // Create markers if mesh collection present
  if (!_markers[D - 1].empty() and !_facet_domains)
    _facet_domains = mesh_function(_markers[D - 1], D - 1, unset_value);

  return _facet_domains;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<MeshFunction<std::size_t> >
MeshDomains::mesh_function(const std::map<std::size_t, std::size_t>& values,
                           std::size_t d, std::size_t unset_value) const
{
  // Get dimensions
  const std::size_t D = _mesh.topology().dim();

  // Create MeshFunction
  boost::shared_ptr<MeshFunction<std::size_t> >
    mesh_function(new MeshFunction<std::size_t>(_mesh, d, unset_value));

  // Get mesh connectivity D --> d
  dolfin_assert(d <= D);

  // Iterate over all values
  std::map<std::size_t, std::size_t>::const_iterator it;
  for (it = values.begin(); it != values.end(); ++it)
  {
    // Get marker data
    const std::size_t entity_index = it->first;
    const std::size_t value = it->second;

    // Check that value is not equal to the 'unset' value
    if (value == unset_value)
      warning("MeshValueCollection value entry is equal to %d, which is used to indicate an \"unset\" value.", value);

    // Set value for entity
    (*mesh_function)[entity_index] = value;
  }

  return mesh_function;
}
//-----------------------------------------------------------------------------
const MeshDomains& MeshDomains::operator= (const MeshDomains& domains)
{
  // Clear all data
  clear();

  // Copy data
  _markers = domains._markers;

  // Reset MeshFunctions
  _cell_domains.reset();
  _facet_domains.reset();

  return *this;
}
//-----------------------------------------------------------------------------
void MeshDomains::init(std::size_t dim)
{
  // Clear old data
  clear();

  // Add markers for each topological dimension
  _markers.resize(dim +1);

  /*
  for (std::size_t d = 0; d <= dim; d++)
  {
    boost::shared_ptr<MeshValueCollection<std::size_t> >
        m(new MeshValueCollection<std::size_t>(d));
    _markers.push_back(m);
  }
  */
}
//-----------------------------------------------------------------------------
void MeshDomains::clear()
{
  _markers.clear();
  _cell_domains.reset();
  _facet_domains.reset();
}
//-----------------------------------------------------------------------------
