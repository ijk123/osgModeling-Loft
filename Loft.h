/* -*-c++-*- osgModeling - Copyright (C) 2008 Wang Rui <wangray84@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.

* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef OSGMODELING_LOFT
#define OSGMODELING_LOFT 1

#include "Model.h"

namespace osgModeling {

/** Loft modeling class
 * Loft build models with at least a shape as the model section and a path.
 * User may add multi shapes to define a complex model but no more than segments of the path.
 * Shapes had better be 2D curves in XY plane, for they are considered so while processing.
 */
class Loft : public Model
{
public:
    typedef std::vector< vEngine::ref_ptr<Curve> > Shapes;

    Loft();
    Loft( const Loft& copy, const vEngine::CopyOp& copyop=vEngine::CopyOp::SHALLOW_COPY );

    /** Specifies parameters in constructor & no need to call update(). Provided for convenience. */
    Loft( Curve* pts, Curve* section );

    META_Object( osgModeling, Loft );

    /** Specifies a vertex list as path of the lofting model. */
    inline void setProfile( Curve* pts ) { _profile=pts; if (_updated) _updated=false; }
    inline Curve* getProfile() { return _profile.get(); }
    inline const Curve* getProfile() const { return _profile.get(); }

    /** Specifies a vertex list as a section of model, placing at specific knot of path. */
    inline void addShape( Curve* pts ) { if ( pts ) _shapes.push_back( pts ); }
    inline void insertShape( Curve* pts, unsigned int pos=0 )
    {
        if ( !pts ) return;
        if ( _shapes.size()<=pos )
            _shapes.resize( pos );
        _shapes.insert( _shapes.begin()+pos, pts );

        if (_updated)
            _updated=false;
    }
    inline Curve* getShape( unsigned int pos=0 ) 
    {
        if ( pos>=_shapes.size() )
            return NULL;
        return _shapes.at(pos).get();
    }
    inline const Curve* getShape( unsigned int pos=0 ) const
    {
        if ( pos>=_shapes.size() )
            return NULL;
        return _shapes.at(pos).get();
    }
    inline Shapes getAllShapes() { return _shapes; }

    virtual void updateImplementation();

protected:
    virtual ~Loft();

    bool processSections( const Curve* path, Loft::Shapes& shapes );
    bool buildTransitions( Loft::Shapes::iterator& from, Loft::Shapes::iterator& to, const vEngine::Vec3* pathPtr );
    vEngine::Vec3 considerBasisX( const vEngine::Vec3 basisZ );

    vEngine::ref_ptr<Curve> _profile;
    Shapes _shapes;
};

}

#endif
