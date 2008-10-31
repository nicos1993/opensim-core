#ifndef __SimbodyEngine01_05_h__
#define __SimbodyEngine01_05_h__
// SimbodyEngine01_05.h
// Authors: Frank C. Anderson, Ajay Seth
/*
 * Copyright (c)  2007, Stanford University. All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// INCLUDES
#include <iostream>
#include <string>
#include "osimSimbodyEngineDLL.h"
#include <OpenSim/Common/Array.h>
#include <OpenSim/Common/Object.h>
#include <OpenSim/Common/PropertyObj.h>
#include <OpenSim/Common/ArrayPtrs.h>
#include <OpenSim/Common/Function.h>
#include <OpenSim/Common/ScaleSet.h>
#include <OpenSim/Simulation/Model/AbstractDynamicsEngine01_05.h>
#include <OpenSim/Simulation/Model/CoordinateSet.h>
#include <SimTKsimbody.h>
#include "SimbodyBody01_05.h"
#include "SimbodyCoordinate01_05.h"
#include "SimbodyJoint01_05.h"
#include "SimbodySpeed01_05.h"

#ifdef SWIG
	#ifdef OSIMSIMBODYENGINE_API
		#undef OSIMSIMBODYENGINE_API
		#define OSIMSIMBODYENGINE_API
	#endif
#endif

namespace OpenSim {

class SimbodyBody01_05;
class SimbodyCoordinate01_05;
class SimbodyJoint01_05;
class SimbodySpeed01_05;
class AbstractDof01_05;
class CoordinateSet;
class AbstractBody;
class AbstractCoordinate;
class Model;
class Transform;

//=============================================================================
//=============================================================================
/**
 * A wrapper class to use the SimTK Simbody dynamics engine as the underlying
 * engine for OpenSim.
 *
 * @authors Frank C. Anderson
 * @version 1.0
 */
class OSIMSIMBODYENGINE_API SimbodyEngine01_05  : public AbstractDynamicsEngine01_05
{

//=============================================================================
// DATA
//=============================================================================
protected:
	/** Body used for ground, the inertial frame. */
	AbstractBody *_groundBody;

	/** Multibody system. */
	SimTK::MultibodySystem *_system;

	/** Matter subsystem. */
	SimTK::SimbodyMatterSubsystem *_matter;

	/** Uniform gravity subsystem. */
	//SimTK::UniformGravitySubsystem *_gravitySubsystem;

	/** User-force subsystem. */
	//SimTK::GeneralForceElements *_userForceElements;

	/** States of the Simbody model.  At a minimum, it contains the
	generalized coordinates (q) and generalized speeds (u). */
	SimTK::State *_s;

	/** Vector of spatial vectors containing the accumulated forces
	and torques that are to be applied to each of the bodies in the
	matter subsystem.  Forces and torques are accumulated in this
	vector when the applyForce()/applyTorque() methods are called. */
	SimTK::Vector_<SimTK::SpatialVec> _bodyForces;

	/** Vector of mobility forces containing the accumulated generalized
	coordinates that are to be applied to the matter subsystem.  The forces
	are accumulated in this vector when the applyGeneralizedForce() methods
	are called. */
	SimTK::Vector _mobilityForces;

//=============================================================================
// METHODS
//=============================================================================
	//--------------------------------------------------------------------------
	// CONSTRUCTION AND DESTRUCTION
	//--------------------------------------------------------------------------
public:
	virtual ~SimbodyEngine01_05();
	SimbodyEngine01_05();
	SimbodyEngine01_05(const std::string &aFileName);
	SimbodyEngine01_05(const SimbodyEngine01_05& aEngine);
	virtual Object* copy() const;
#ifndef SWIG
	SimbodyEngine01_05& operator=(const SimbodyEngine01_05 &aEngine);
#endif
	static void registerTypes();

private:
	void setNull();
	void setupProperties();
	void copyData(const SimbodyEngine01_05 &aEngine);
	void newSimbodyVariables();
	void deleteSimbodyVariables();
	void constructPendulum();
	void constructMultibodySystem();
	void addRigidBodies(SimbodyBody01_05 *aBody);
	int findIndexOfDofThatHasLastGeneralizedCoordinate(DofSet01_05 *aDofSet);
	void createGroundBodyIfNecessary();
	SimbodyJoint01_05* getInboardTreeJoint(SimbodyBody01_05 *aBody) const;
	SimbodyJoint01_05* getOutboardTreeJoint(SimbodyBody01_05 *aBody,int &rIndex) const;	

public:
	void init(Model* aModel);
	virtual void setup(Model* aModel);

	//--------------------------------------------------------------------------
	// GRAVITY
	//--------------------------------------------------------------------------
	virtual bool setGravity(const SimTK::Vec3& aGrav);
	virtual void getGravity(SimTK::Vec3& rGrav) const;

	//--------------------------------------------------------------------------
   // ADDING COMPONENTS
	//--------------------------------------------------------------------------
	void addBody(SimbodyBody01_05* aBody);
	void addJoint(SimbodyJoint01_05* aJoint);
	void addCoordinate(SimbodyCoordinate01_05* aCoord);
	void addSpeed(SimbodySpeed01_05* aSpeed);

	//--------------------------------------------------------------------------
	// COORDINATES
	//--------------------------------------------------------------------------
	virtual void updateCoordinateSet(CoordinateSet& aCoordinateSet);
	virtual void getUnlockedCoordinates(CoordinateSet& rUnlockedCoordinates) const;
	virtual AbstractDof01_05* findUnconstrainedDof(const AbstractCoordinate& aCoordinate,
		AbstractJoint*& rJoint) { return NULL; }

	//--------------------------------------------------------------------------
	// CONFIGURATION
	//--------------------------------------------------------------------------
	virtual void setTime(const double aTime) {_s->setTime(aTime); };
	virtual void setConfiguration(const double aY[]);
	virtual void getConfiguration(double rY[]) const;
	virtual void setConfiguration(const double aQ[], const double aU[]);
	virtual void getConfiguration(double rQ[],double rU[]) const;
	virtual void getCoordinates(double rQ[]) const;
	virtual void getSpeeds(double rU[]) const;
	virtual void getAccelerations(double rDUDT[]) const;
	virtual void extractConfiguration(const double aY[], double rQ[], double rU[]) const;
	virtual void applyDefaultConfiguration();
	double* getConfiguration();
	double* getDerivatives();

	//--------------------------------------------------------------------------
	// ASSEMBLING THE MODEL
	//--------------------------------------------------------------------------
	virtual int
		assemble(double aTime,double *rState,int *aLock,double aTol,
		int aMaxevals,int *rFcnt,int *rErr) { return 0; };

	//--------------------------------------------------------------------------
	// SCALING
	//--------------------------------------------------------------------------
	virtual bool scale(const ScaleSet& aScaleSet, double aFinalMass = -1.0, bool aPreserveMassDist = false);

	//--------------------------------------------------------------------------
	// BODY INFORMATION
	//--------------------------------------------------------------------------
	virtual AbstractBody& getGroundBody() const;
	virtual AbstractBody* getLeafBody(AbstractJoint* aJoint) const { return NULL; }
	bool adjustJointVectorsForNewMassCenter(SimbodyBody01_05* aBody);

	//--------------------------------------------------------------------------
	// INERTIA
	//--------------------------------------------------------------------------
	virtual double getMass() const;
	virtual void getSystemInertia(double *rM, SimTK::Vec3& rCOM, double rI[3][3]) const;
	virtual void getSystemInertia(double *rM, double *rCOM, double *rI) const;

	//--------------------------------------------------------------------------
	// KINEMATICS
	//--------------------------------------------------------------------------
	virtual void getPosition(const AbstractBody &aBody, const SimTK::Vec3& aPoint, SimTK::Vec3& rPos) const;
	virtual void getVelocity(const AbstractBody &aBody, const SimTK::Vec3& aPoint, SimTK::Vec3& rVel) const;
	virtual void getAcceleration(const AbstractBody &aBody, const SimTK::Vec3& aPoint, SimTK::Vec3& rAcc) const;
	virtual void getDirectionCosines(const AbstractBody &aBody, double rDirCos[3][3]) const;
	virtual void getDirectionCosines(const AbstractBody &aBody, double *rDirCos) const;
	virtual void getAngularVelocity(const AbstractBody &aBody, SimTK::Vec3& rAngVel) const;
	virtual void getAngularVelocityBodyLocal(const AbstractBody &aBody, SimTK::Vec3& rAngVel) const;
	virtual void getAngularAcceleration(const AbstractBody &aBody, SimTK::Vec3& rAngAcc) const;
	virtual void getAngularAccelerationBodyLocal(const AbstractBody &aBody, SimTK::Vec3& rAngAcc) const;
	virtual OpenSim::Transform getTransform(const AbstractBody &aBody);

	//--------------------------------------------------------------------------
	// LOAD APPLICATION
	//--------------------------------------------------------------------------
	// FORCES EXPRESSED IN INERTIAL FRAME
	virtual void applyForce(const AbstractBody &aBody, const SimTK::Vec3& aPoint, const SimTK::Vec3& aForce);
	virtual void applyForces(int aN, const AbstractBody *aBodies[], const double aPoints[][3], const double aForces[][3]);
	virtual void applyForces(int aN, const AbstractBody *aBodies[], const double *aPoints, const double *aForces);

	// FORCES EXPRESSED IN BODY-LOCAL FRAME
	virtual void applyForceBodyLocal(const AbstractBody &aBody, const SimTK::Vec3& aPoint, const SimTK::Vec3& aForce);
	virtual void applyForcesBodyLocal(int aN, const AbstractBody *aBodies[], const double aPoints[][3], const double aForces[][3]);
	virtual void applyForcesBodyLocal(int aN, const AbstractBody *aBodies[], const double *aPoints, const double *aForces);

	// TORQUES EXPRESSED IN INERTIAL FRAME
	virtual void applyTorque(const AbstractBody &aBody, const SimTK::Vec3& aTorque);
	virtual void applyTorques(int aN, const AbstractBody *aBodies[], const double aTorques[][3]);
	virtual void applyTorques(int aN, const AbstractBody *aBodies[], const double *aTorques);

	// TORQUES EXPRESSED IN BODY-LOCAL FRAME
	virtual void applyTorqueBodyLocal(const AbstractBody &aBody, const SimTK::Vec3& aTorque);
	virtual void applyTorquesBodyLocal(int aN, const AbstractBody *aBodies[], const double aTorques[][3]);
	virtual void applyTorquesBodyLocal(int aN, const AbstractBody *aBodies[], const double *aTorques);

	// GENERALIZED FORCES
	virtual void applyGeneralizedForce(const AbstractCoordinate &aU, double aF);
	virtual void applyGeneralizedForces(const double aF[]);
	virtual void applyGeneralizedForces(int aN, const AbstractCoordinate *aU[], const double aF[]);

	//--------------------------------------------------------------------------
	// LOAD ACCESS AND COMPUTATION
	//--------------------------------------------------------------------------
	virtual double getNetAppliedGeneralizedForce(const AbstractCoordinate &aU) const;
	virtual void computeGeneralizedForces(double aDUDT[], double rF[]) const;
	virtual void computeReactions(double rForces[][3], double rTorques[][3]) const;

	//--------------------------------------------------------------------------
	// CONSTRAINTS
	//--------------------------------------------------------------------------
	virtual void computeConstrainedCoordinates(double rQ[]) const {};

	//--------------------------------------------------------------------------
	// EQUATIONS OF MOTION
	//--------------------------------------------------------------------------
	virtual void formMassMatrix(double *rI) {};
	virtual void formEulerTransform(const AbstractBody &aBody, double *rE) const;
	virtual void formJacobianTranslation(const AbstractBody &aBody, const SimTK::Vec3& aPoint, double *rJ, const AbstractBody *aRefBody=NULL) const {};
	virtual void formJacobianOrientation(const AbstractBody &aBody, double *rJ0, const AbstractBody *aRefBody=NULL) const {};
	virtual void formJacobianEuler(const AbstractBody &aBody, double *rJE, const AbstractBody *aRefBody=NULL) const {};

	//--------------------------------------------------------------------------
	// DERIVATIVES
	//--------------------------------------------------------------------------
	virtual void computeDerivatives(double *dqdt,double *dudt);

	//--------------------------------------------------------------------------
	// UTILITY
	//--------------------------------------------------------------------------
	virtual void transform(const AbstractBody &aBodyFrom, const double aVec[3], const AbstractBody &aBodyTo, double rVec[3]) const;
	virtual void transform(const AbstractBody &aBodyFrom, const SimTK::Vec3& aVec, const AbstractBody &aBodyTo, SimTK::Vec3& rVec) const;
	virtual void transformPosition(const AbstractBody &aBodyFrom, const double aPos[3], const AbstractBody &aBodyTo, double rPos[3]) const;
	virtual void transformPosition(const AbstractBody &aBodyFrom, const SimTK::Vec3& aPos, const AbstractBody &aBodyTo, SimTK::Vec3& rPos) const;
	virtual void transformPosition(const AbstractBody &aBodyFrom, const double aPos[3], double rPos[3]) const;
	virtual void transformPosition(const AbstractBody &aBodyFrom, const SimTK::Vec3& aPos, SimTK::Vec3& rPos) const;

	virtual double calcDistance(const AbstractBody &aBody1, const double aPoint1[3], const AbstractBody &aBody2, const double aPoint2[3]) const;
	virtual double calcDistance(const AbstractBody &aBody1, const SimTK::Vec3& aPoint1, const AbstractBody &aBody2, const SimTK::Vec3& aPoint2) const;

	virtual void convertQuaternionsToAngles(double *aQ, double *rQAng) const;
	virtual void convertQuaternionsToAngles(Storage *rQStore) const;
	virtual void convertAnglesToQuaternions(double *aQAng, double *rQ) const;
	virtual void convertAnglesToQuaternions(Storage *rQStore) const;

	virtual void convertAnglesToDirectionCosines(double aE1, double aE2, double aE3, double rDirCos[3][3]) const;
	virtual void convertAnglesToDirectionCosines(double aE1, double aE2, double aE3, double *rDirCos) const;

	virtual void convertDirectionCosinesToAngles(double aDirCos[3][3], double *rE1, double *rE2, double *rE3) const;
	virtual void convertDirectionCosinesToAngles(double *aDirCos, double *rE1, double *rE2, double *rE3) const;

	virtual void convertDirectionCosinesToQuaternions(double aDirCos[3][3],	double *rQ1, double *rQ2, double *rQ3, double *rQ4) const;
	virtual void convertDirectionCosinesToQuaternions(double *aDirCos, double *rQ1, double *rQ2, double *rQ3, double *rQ4) const;

	virtual void convertQuaternionsToDirectionCosines(double aQ1, double aQ2, double aQ3, double aQ4, double rDirCos[3][3]) const;
	virtual void convertQuaternionsToDirectionCosines(double aQ1, double aQ2, double aQ3, double aQ4, double *rDirCos) const;

	// INTERFACE TO THE SIMBODY FORCE SUBSYSTEM
	void resizeBodyAndMobilityForceVectors();
	void resetBodyAndMobilityForceVectors();
	const SimTK::Vector_<SimTK::SpatialVec>& getBodyForces() { return _bodyForces; }
	const SimTK::Vector& getMobilityForces() { return _mobilityForces; }

private:
	friend class SimbodyBody01_05;
	friend class SimbodyCoordinate01_05;
	friend class SimbodySpeed01_05;
	friend class SimbodyJoint01_05;

//=============================================================================
};	// END of class SimbodyEngine01_05
//=============================================================================
//=============================================================================

} // end of namespace OpenSim

//=============================================================================
// STATIC METHOD FOR CREATING THIS MODEL
//=============================================================================
extern "C" {
OSIMSIMBODYENGINE_API OpenSim::Model* CreateModel();
OSIMSIMBODYENGINE_API OpenSim::Model* CreateModel_File(const std::string &aModelFile);
}

#endif // __SimbodyEngine01_05_h__

