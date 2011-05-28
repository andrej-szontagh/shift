
////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDE
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

#include "btBulletDynamicsCommon.h"

#include	    "types.h"
#include	    "macro.h"
#include	 "skeleton.h"
#include	  "physics.h"

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

#define M_PH_GRAVITY 10.0f

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

btDefaultCollisionConfiguration		* ph_configuration;
btBroadphaseInterface				* ph_broadphase;
btCollisionDispatcher				* ph_dispatcher;
btSequentialImpulseConstraintSolver	* ph_solver;
btDiscreteDynamicsWorld				* ph_world;

btAlignedObjectArray <btCollisionShape *> ph_shapes;

btClock ph_clock;

btCollisionShape *ph_shapeground;
btCollisionShape *ph_shapecrate;

btRigidBody *ph_bodyground;
btRigidBody *ph_bodycrate;

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// ph_Init
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

void ph_Init ()
{

	//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	ph_configuration	= new btDefaultCollisionConfiguration ();

	//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	ph_dispatcher		= new btCollisionDispatcher (ph_configuration);

	//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	ph_broadphase		= new btDbvtBroadphase ();

	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	ph_solver			= new btSequentialImpulseConstraintSolver;

	// creating world
	ph_world			= new btDiscreteDynamicsWorld (ph_dispatcher, ph_broadphase, ph_solver, ph_configuration);

	// setting gravity
	ph_world->setGravity (btVector3 (0, - M_PH_GRAVITY, 0));

	// OBJECTS

	btScalar mass;

	btVector3 inertia (0, 0, 0);

	// GROUND

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	mass = 0.0;

	//ground shape
	ph_shapeground = new btBoxShape (btVector3 (500.0f, 0.1f, 500.0f));

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	ph_shapes.push_back (ph_shapeground);

	btTransform groundTransform;

	groundTransform.setIdentity ();
	groundTransform.setOrigin (btVector3 (0.0f, 0.0f, 0.0f));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btRigidBody::btRigidBodyConstructionInfo rbinfo_ground (mass, new btDefaultMotionState (groundTransform), ph_shapeground, inertia);

	// rigid body
	ph_bodyground = new btRigidBody (rbinfo_ground);

	//add the body to the dynamics world
	ph_world->addRigidBody (ph_bodyground);

	// CRATE

	//create a dynamic rigidbody
	ph_shapecrate = new btSphereShape (1.0f);

	ph_shapes.push_back (ph_shapecrate);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	mass = 1.0f;

	// Create Dynamic Objects
	btTransform crateTransform;
	
	crateTransform.setIdentity ();
	crateTransform.setOrigin (btVector3 (2.0f, 50.0f, 0.0f));

	ph_shapecrate->calculateLocalInertia (mass, inertia);
	
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btRigidBody::btRigidBodyConstructionInfo rbinfo_crate (mass, new btDefaultMotionState (crateTransform), ph_shapecrate, inertia);

	// rigid body
	ph_bodycrate = new btRigidBody (rbinfo_crate);

	//add the body to the dynamics world
	ph_world->addRigidBody (ph_bodycrate);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// ph_Step
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

void ph_Step ()
{
	ph_world->stepSimulation (	ph_clock.getTimeMicroseconds () / 1000000.0f, 10);	
								ph_clock.reset ();

	btTransform trans;	ph_bodycrate->getMotionState ()->getWorldTransform (trans);

	FLOAT_32 mat [16];

	mIDENTITY  (mat);
	mTRANSLATE (mat, float(trans.getOrigin().getX()), 
					 float(trans.getOrigin().getY()), 
					 float(trans.getOrigin().getZ()));

///	mTRANSPOSE (prefab_budha.matrix, mat);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// ph_Free
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

void ph_Free ()
{
	INT_32 i, j;

	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	for (i = ph_world->getNumCollisionObjects () - 1; i >= 0; i --) {

		btCollisionObject	* obj  = ph_world->getCollisionObjectArray () [i];
		btRigidBody			* body = btRigidBody::upcast (obj);

		if (body && body->getMotionState ())	delete body->getMotionState ();

		ph_world->removeCollisionObject (obj);	delete obj;
	}

	//delete collision shapes
	for (j = 0; j < ph_shapes.size (); j ++) {

		btCollisionShape* shape = ph_shapes [j];	ph_shapes [j] = 0;
		delete shape;
	}

	delete ph_world;
	delete ph_solver;
	delete ph_broadphase;
	delete ph_dispatcher;
	delete ph_configuration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	ph_shapes.clear ();
}

#endif
