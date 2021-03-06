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

#ifndef OSGMODELING_MODEL
#define OSGMODELING_MODEL 1

#include <iostream>
#include <vEngine/CopyOp.h>
#include <vEngine/Geometry.h>
#include "BspTree.h"
#include "NormalVisitor.h"
#include "TexCoordVisitor.h"
#include "Curve.h"

namespace osgModeling {

/** Modeling base class
 * This is the base class of all osgModeling models.
 */
class Model : public vEngine::Geometry
{
public:
    enum GenerateParts { CAP1_PART=0x1, BODY_PART=0x2, CAP2_PART=0x4, ALL_PARTS=CAP1_PART|BODY_PART|CAP2_PART };
    enum GenerateCoords { NORMAL_COORDS=0x1, TEX_COORDS=0x2, ALL_COORDS=NORMAL_COORDS|TEX_COORDS };
    enum AuxFunctions { FLIP_NORMAL=0x1, USE_WIREFRAME=0x2 };

    Model():
        vEngine::Geometry(),
        _updated(false), _partsToGenerate(BODY_PART), _coordsToGenerate(ALL_COORDS), _funcs(0),
        _algorithmCallback(0), _normalGenerator(0), _texCoordGenerator(0), _bspTree(0)
    {
    }

    Model( const vEngine::Geometry& copy, const vEngine::CopyOp& copyop=vEngine::CopyOp::SHALLOW_COPY ):
        vEngine::Geometry(copy,copyop),
        _updated(true), _funcs(0), _algorithmCallback(0),
        _normalGenerator(0), _texCoordGenerator(0), _bspTree(0)
    {
    }

    Model( const Model& copy, const vEngine::CopyOp& copyop=vEngine::CopyOp::SHALLOW_COPY ):
        vEngine::Geometry(copy,copyop),
        _updated(copy._updated), _partsToGenerate(copy._partsToGenerate), _coordsToGenerate(copy._coordsToGenerate),
        _funcs(copy._funcs), _algorithmCallback(copy._algorithmCallback), _normalGenerator(copy._normalGenerator),
        _texCoordGenerator(copy._texCoordGenerator), _bspTree(copy._bspTree)
    {
    }

    META_Object( osgModeling, Model );

    /** Set which part of the model should be generated. Use 'OR' operation to select from enum GenerateParts. */
    inline void setGenerateParts( int gp=ALL_PARTS )
    {
        if ( _partsToGenerate!=gp )
        {
            _updated = false;
            _partsToGenerate = gp;
        }
    }
    inline int getGenerateParts() const { return _partsToGenerate; }

    /** Set whether to generate normal/texture coords from enum GenerateCoords. */
    inline void setGenerateCoords( int gc=ALL_COORDS )
    {
        if ( _coordsToGenerate!=gc )
        {
            _updated = false;
            _coordsToGenerate = gc;
        }
    }
    inline int getGenerateCoords() const { return _coordsToGenerate; }

    /** Set auxiliary functions to help build model for special uses.
     * There are some functions to select from enum AuxFunctions:
     * - FLIP_NORMAL: Flip the generated normals.
     * - USE_WIREFRAME: Show wire-frame of the model instead of solid one.
     *  Use 'OR' operation to select more than one functions.
     */
    inline void setAuxFunctions( int funcs ) { _funcs=funcs; }
    inline int getAuxFunctions() { return _funcs; }

    /** Set the geometry generating algorithm to use.
     * Every inherited model class has a default algorithm to create vertics, normals and texture coordinates.
     * User may easily inherit AlgorithmCallback to realize better algorithms, and set it to the model class.
     */
    inline void setAlgorithmCallback( AlgorithmCallback* ac ) { _algorithmCallback=ac; }
    inline AlgorithmCallback* getAlgorithmCallback() { return _algorithmCallback.get(); }

    /** Set the normal generator to use, or there will be default one. */
    inline void setNormalGenerator( NormalVisitor* nv ) { _normalGenerator=nv; }
    inline NormalVisitor* getNormalGenerator() { return _normalGenerator.get(); }

    /** Set the texture coordinates generator to use, or there will be default one. */
    inline void setTexCoordGenerator( TexCoordVisitor* tcv ) { _texCoordGenerator=tcv; }
    inline TexCoordVisitor* getTexCoordGenerator() { return _texCoordGenerator.get(); }

    /** Attach a BSP tree to the model. */
    inline void setBspTree( BspTree* bsp ) { _bspTree=bsp; }
    inline BspTree* getBspTree() { return _bspTree.get(); }

    /** Call this before drawing to generate primitives.
     * If need to be modified while running, the object should set to DYNAMIC.
     * \param forceUpdate Set to true to force rebuilding, otherwise the function may be ignored because nothing changed.
     */
    virtual void update( bool forceUpdate=false )
    {
        if ( _updated && !forceUpdate )
            return;

        if ( _algorithmCallback.valid() )
            (*_algorithmCallback)( this );
        else
            updateImplementation();

        _updated = true;
    }

    virtual void updateImplementation() {}

    virtual void drawImplementation( vEngine::RenderInfo &renderInfo ) const
    {
        //if ( !_updated )
        //    vEngine::notify(vEngine::WARN) << "osgModeling::" << className() << ": Call update() to update changed models." <<std::endl;

        vEngine::Geometry::drawImplementation( renderInfo );
    }

protected:
    virtual ~Model() {}

    bool _updated;

    int _partsToGenerate;
    int _coordsToGenerate;
    int _funcs;

    vEngine::ref_ptr<AlgorithmCallback> _algorithmCallback;
    vEngine::ref_ptr<NormalVisitor> _normalGenerator;
    vEngine::ref_ptr<TexCoordVisitor> _texCoordGenerator;
    vEngine::ref_ptr<BspTree> _bspTree;
};

}

#endif
