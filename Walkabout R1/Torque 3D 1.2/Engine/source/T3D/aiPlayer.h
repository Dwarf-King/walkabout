//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#ifndef _AIPLAYER_H_
#define _AIPLAYER_H_

#ifndef _PLAYER_H_
#include "T3D/player.h"
#endif

#include "walkabout/navPath.h"
#include "walkabout/navMesh.h"
#include "walkabout/coverPoint.h"

class AIPlayer : public Player {

	typedef Player Parent;

public:
	enum MoveState {
		ModeStop,                       // AI has stopped moving.
		ModeMove,                       // AI is currently moving.
		ModeStuck,                      // AI is stuck, but wants to move.
      ModeSlowing,                    // AI is slowing down as it reaches it's destination.
	};

private:
   MoveState mMoveState;
   F32 mMoveSpeed;
   F32 mMoveTolerance;                 // Distance from destination before we stop
   Point3F mMoveDestination;           // Destination for movement
   Point3F mLastLocation;              // For stuck check
   F32 mMoveStuckTolerance;            // Distance tolerance on stuck check
   S32 mMoveStuckTestDelay;            // The number of ticks to wait before checking if the AI is stuck
   S32 mMoveStuckTestCountdown;        // The current countdown until at AI starts to check if it is stuck
   bool mMoveSlowdown;                 // Slowdown as we near the destination

   SimObjectPtr<GameBase> mAimObject; // Object to point at, overrides location
   bool mAimLocationSet;               // Has an aim location been set?
   Point3F mAimLocation;               // Point to look at
   bool mTargetInLOS;                  // Is target object visible?

   Point3F mAimOffset;

   // Utility Methods
   void throwCallback( const char *name );

   /// Should we jump?
   enum JumpStates {
      None,  ///< No, don't jump.
      Now,   ///< Jump immediately.
      Ledge, ///< Jump when we walk off a ledge.
   } mJump;

   /// Stores information about a path.
   struct PathData {
      /// Pointer to path object.
      SimObjectPtr<Nav::NavPath> path;
      /// Do we own our path? If so, we will delete it when finished.
      bool owned;
      /// Path node we're at.
      U32 index;
      /// Default constructor.
      PathData() : path(NULL)
      {
         owned = false;
         index = 0;
      }
   };

   /// Path we are currently following.
   PathData mPathData;

   /// Clear out the current path.
   void clearPath();

   /// Get the current path we're following.
   Nav::NavPath *getPath() { return mPathData.path; }

   /// Stores information about our cover.
   struct CoverData {
      /// Pointer to a cover point.
      SimObjectPtr<CoverPoint> cover;
      /// Default constructor.
      CoverData() : cover(NULL)
      {
      }
   };

   /// Current cover we're trying to get to.
   CoverData mCoverData;

   /// Stop searching for cover.
   void clearCover();

   /// Get cover we are moving to.
   CoverPoint *getCover() { return mCoverData.cover; }

   /// Information about a target we're following.
   struct FollowData {
      /// Object to follow.
      SimObjectPtr<SceneObject> object;
      /// Distance at whcih to follow.
      F32 radius;
      /// Default constructor.
      FollowData() : object(NULL)
      {
         radius = 5.0f;
      }
   };

   /// Current object we're following.
   FollowData mFollowData;

   /// Stop following me!
   void clearFollow();

   /// NavMesh we pathfind on.
   SimObjectPtr<Nav::NavMesh> mNavMesh;

   /// Move to the specified node in the current path.
   void moveToNode(S32 node);

protected:
   virtual void onReachDestination();
   virtual void onStuck();

public:
   DECLARE_CONOBJECT( AIPlayer );

   AIPlayer();
   ~AIPlayer();

   static void initPersistFields();

   bool onAdd();
   void onRemove();

   virtual bool getAIMove( Move *move );

   // Targeting and aiming sets/gets
   void setAimObject( GameBase *targetObject );
   void setAimObject( GameBase *targetObject, Point3F offset );
   GameBase* getAimObject() const  { return mAimObject; }
   void setAimLocation( const Point3F &location );
   Point3F getAimLocation() const { return mAimLocation; }
   void clearAim();

   // Movement sets/gets
   void setMoveSpeed( const F32 speed );
   F32 getMoveSpeed() const { return mMoveSpeed; }
   void setMoveTolerance( const F32 tolerance );
   F32 getMoveTolerance() const { return mMoveTolerance; }
   void setMoveDestination( const Point3F &location, bool slowdown );
   Point3F getMoveDestination() const { return mMoveDestination; }
   void stopMove();

   /// @name Pathfinding
   /// @{

   enum NavSize {
      Small,
      Regular,
      Large
   } mNavSize;
   void setNavSize(NavSize size) { mNavSize = size; updateNavMesh(); }
   NavSize getNavSize() const { return mNavSize; }

   bool setPathDestination(const Point3F &pos);
   Point3F getPathDestination() const;

   void followNavPath(Nav::NavPath *path);
   void followObject(SceneObject *obj, F32 radius);

   void repath();

   bool findCover(const Point3F &from, F32 radius);

   Nav::NavMesh *findNavMesh() const;
   void updateNavMesh();
   Nav::NavMesh *getNavMesh() const { return mNavMesh; }

   /// Types of link we can use.
   Nav::LinkData mLinkTypes;

   /// @}
};

#endif
