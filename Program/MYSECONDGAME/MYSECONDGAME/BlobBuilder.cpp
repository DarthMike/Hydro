/*
	Filename: BlobBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a blob (type of soft body simulation with rigid bodies engine)
				 given some parameters. It creates a new group of bodies, associated to a game agent
				 provided by the client of the class
	Comments: Uses Box2D and its created class to manage it. Many resources from the net used, but it is based in big part
	          on a structure created and tried before: http://cowboyprogramming.com/2007/01/05/blob-physics/
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "BlobBuilder.h"
#include "AgentsManager.h"
#include "PhysicsManager.h"
#include "IndieLibManager.h"

//Definition of static variables from class
int BlobBuilder::mBlobsCreated = 0;

//Read a part of xml info and create a blob object
void BlobBuilder::LoadBlob(const ticpp::Element* xmlelement)  
{
	/* 
	XML DEFINITION EXPECTED FOR BLOB ELEMENT 
 	Element: AI Atts: Id (string) x(number) y(number) Layer(number) Radius(number) InnerSkinRadius(number)
				Bodies(number) InJointsDamping(number) InJointsFrequency(number) SkinJointsDamping(number) SkinJointsFrequency(number) DoubleSkin(number)
				MassesRadius(number) MassesFriction(number) MassesRestitution(number)
				MassesDensity(number)
				InnerMassRadius(number) InnerMassDensity(number)
	*/
	
	//Assert correct parameters
	std::string entitytype;
	xmlelement->GetValue(&entitytype);
	assert( entitytype == "Blob");

	//-------Get attributes from entity-------------
	
	//Parameters of creation
	BlobParameters creationparams; //All blob-related params
	int layer(62);
	xmlelement->GetAttribute("x",&creationparams.initialx);
	xmlelement->GetAttribute("y",&creationparams.initialy);
	xmlelement->GetAttribute("Layer",&layer);
	xmlelement->GetAttribute("Radius",&creationparams.radius);
	xmlelement->GetAttribute("Bodies",&creationparams.bodies);
	xmlelement->GetAttribute("DamageFilterTime",&creationparams.damagefiltertime);
	xmlelement->GetAttribute("InJointsDamping",&creationparams.jointsdamping);
	xmlelement->GetAttribute("InJointsFrequency",&creationparams.jointsfrequency);
	xmlelement->GetAttribute("SkinJointsDamping",&creationparams.skinjointsdamping);
	xmlelement->GetAttribute("SkinJointsFrequency",&creationparams.skinjointsfrequency);
	xmlelement->GetAttribute("DoubleSkin",&creationparams.doubleskinned);
	xmlelement->GetAttribute("MassesRadius",&creationparams.massesradius);
	xmlelement->GetAttribute("MassesFriction",&creationparams.massesfriction);
	xmlelement->GetAttribute("MassesRestitution",&creationparams.massesrestitution);	
	xmlelement->GetAttribute("MassesDensity",&creationparams.massesdensity);	
	xmlelement->GetAttribute("InnerMassRadius",&creationparams.innermassradius);
	xmlelement->GetAttribute("InnerMassDensity",&creationparams.innermassdensity);

	//Get optional parameter if double skin is active
	if(creationparams.doubleskinned)
		xmlelement->GetAttribute("InnerSkinRadius",&creationparams.innerskinradius);

	//TODO: CHECK ALL ATTRIBUTES!
	//if(x<0 || y<0 || h<=0 || w<=0 || layer<0 || layer > 63 || lindamping < 0 || lindamping > 1 || angdamping < 0 || angdamping > 1 || (isstatic != 0 && isstatic != 1))
	//	throw GenericException("Failure while reading '" + filepath + "' bad attributes detected! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	//Create blob!
	_load(creationparams);
}

//Create a blob with some parameters
void  BlobBuilder::LoadBlob(const BlobParameters& params)
{
	//Create the blob!
	_load(params);
}

void BlobBuilder::_load(const BlobParameters& creationparams)
{
	//--------Building of a blob soft body!!----------
	//Creation of an object controller which will hold all created data together:
	mBlobControllerptr = BlobControllerPointer(new BlobController(mRelatedAgent,mPhysicsMgr));  //Stored in a shared pointer
	mBlobControllerptr->SetInitialParameters(creationparams);
	
	//Total mass of blob
	float totalblobmass(0.0f);   //Store of total computed mass of all bodies together in blob

	//Creation of a physic soft body composed of some masses connected by springs
	b2Vec2 creationrotation(creationparams.radius , 0);  //outer (or only) skin
	b2Vec2 innercreationrotation(creationparams.innerskinradius,0); //inner skin (if selected)
	b2Vec2 creationoffset(creationparams.initialx,creationparams.initialy);
	//Rotation angle to fit all bodies inside radius
	float rotationangle = static_cast<float>(SingletonMath::Instance()->Two_Pi / creationparams.bodies); 
	//Precompute how much to rotate insertion position vector for each mass
	b2Mat22 rotationmatrix(rotationangle);
	
	//*****Circle shapes definition***********
	//All elements (shapes) in a blob are bodies with one circle shape associated to them
	b2CircleDef circledefinition;
	circledefinition.friction = creationparams.massesfriction;
	circledefinition.restitution = creationparams.massesrestitution;
	circledefinition.isSensor = false;
	circledefinition.radius = creationparams.massesradius;
	circledefinition.density = creationparams.massesdensity;
	//TODO: MERGE WITH GROUPS DEFINITIONS IN OVERALL SYSTEM!
	circledefinition.filter.groupIndex = -1; //Never collide * But collide with center mass
	circledefinition.filter.categoryBits = 0x02;
	circledefinition.filter.maskBits = 0x03; //Collide with everything but themselves
			
	//*****Inner mass body definition and creation********
	b2BodyDef innerbodydefinition;
	innerbodydefinition.position = creationoffset;  //Positioned in middle of blob
	innerbodydefinition.allowSleep = true;
	innerbodydefinition.isBullet = false;
	innerbodydefinition.linearDamping = 0;
	innerbodydefinition.angularDamping = 0;
	innerbodydefinition.fixedRotation = true;
	innerbodydefinition.applyPosCorrection = false;		//Hack to Box2D to work correctly with friction and soft bodies
	std::stringstream innerbodyname;
	innerbodyname<<"Blob"<<mBlobsCreated<<"InnerMass";
	//Create the body
	b2Body* centerbody = mPhysicsMgr->CreateBody(&innerbodydefinition,innerbodyname.str());
	assert(centerbody);
	//Assign user data pointer
	centerbody->SetUserData(mRelatedAgent);
	//Create shape for body, with different mass and radius
	b2CircleDef innercircledefinition = circledefinition;
	innercircledefinition.density = creationparams.innermassdensity;
	innercircledefinition.radius = creationparams.innermassradius;
	//TODO: MERGE WITH GROUPS DEFINITIONS IN OVERALL SYSTEM!
	innercircledefinition.filter.groupIndex = 1; //Never collide * But collide with outer masses
	innercircledefinition.filter.categoryBits = 0x02;
	innercircledefinition.filter.maskBits = 0x03; 
	mPhysicsMgr->CreateCircleShape(&innercircledefinition,innerbodyname.str());
	centerbody->SetMassFromShapes();
	totalblobmass += centerbody->GetMass();
	
	//*****Spring joints predefinition***********
	//Generic spring joint from center to skin (inner or outer depending of configuration)
	b2DistanceJointDef springdef;  //Spring joint
	springdef.body1 = NULL;
	springdef.body2 = NULL;
	springdef.collideConnected = true;
	springdef.dampingRatio = creationparams.jointsdamping;
	springdef.frequencyHz = creationparams.jointsfrequency;
	if(!creationparams.doubleskinned)
		springdef.length = creationparams.radius; //outer skin connected to inner mass
	else
		springdef.length = (creationparams.radius - creationparams.innerskinradius); //outer skin connected to inner skin
	springdef.localAnchor1 = b2Vec2(0,0);
	springdef.localAnchor2 = b2Vec2(0,0);

	//Spring joint from skin mass to skin mass
	b2DistanceJointDef skinspringdef = springdef;
	skinspringdef.collideConnected = false;
	//Get distance from masses
	b2Vec2 distance = b2Mul(rotationmatrix,creationrotation) -  creationrotation;
	skinspringdef.length = distance.Length();//Different resting length
	skinspringdef.dampingRatio = creationparams.skinjointsdamping;
	skinspringdef.frequencyHz = creationparams.skinjointsfrequency;

	//Spring between skins (optional) If double skinned, 2 additional springs
	b2DistanceJointDef inskinspringdef = skinspringdef;
	b2DistanceJointDef incrossedspringdef = skinspringdef;
	if(creationparams.doubleskinned)
	{
		//Get distance from masses for crossed junction
		b2Vec2 skindistance = b2Mul(rotationmatrix,creationrotation) -  innercreationrotation;
		float32 innerskindist = skindistance.Length();
		incrossedspringdef.length = innerskindist;
		//Get distance from masses for simple junction
		skindistance = creationrotation - innercreationrotation;
		innerskindist = skindistance.Length();
		inskinspringdef.length = innerskindist;
	}


	//****Creation of blob************

	//Pre-create bodies pointers for future reference inside the creation loop
	//Outer Skin
	b2Body* newbody;
	b2Body* prevbody;
	b2Body* firstbody;
	//Inner Skin
	b2Body* in_newbody;
	b2Body* in_prevbody;
	b2Body* in_firstbody;
	newbody = prevbody = firstbody = NULL;
	in_newbody = in_prevbody = in_firstbody = NULL;
	//Pre-create temporal memory variables of inner loop
	std::stringstream bodyname; //outer skin names
	std::string prevbodyname;
	std::string firstbodyname;
	std::stringstream in_bodyname; //inner skin names
	std::string in_prevbodyname;
	std::string in_firstbodyname;
	//LOOP - Create all bodies in outer skin
	for(int i = 1; i<=creationparams.bodies; i++)
	{
		//*****Creation of new body*******
		b2BodyDef bodydefinition;
		b2Vec2 newposition = creationrotation + creationoffset;
		bodydefinition.position = newposition;
		bodydefinition.allowSleep = true;
		bodydefinition.isBullet = true;
		bodydefinition.linearDamping = 0;
		bodydefinition.angularDamping = 0;
		bodydefinition.fixedRotation = true;
		bodydefinition.applyPosCorrection = false;		//Hack to Box2D to work correctly with friction and soft bodies
		bodyname<<"Blob"<<mBlobsCreated<<"SubMass"<<i;
		newbody = mPhysicsMgr->CreateBody(&bodydefinition,bodyname.str());
		assert(newbody);
		mPhysicsMgr->CreateCircleShape(&circledefinition,bodyname.str());
		newbody->SetMassFromShapes();
		totalblobmass += newbody->GetMass();
		//Assign user data pointer
		newbody->SetUserData(mRelatedAgent);

		//*****Creation of inner skin body*******
		if(creationparams.doubleskinned)
		{
			in_bodyname<<"Blob"<<mBlobsCreated<<"SubMassInner"<<i;
			bodydefinition.position = innercreationrotation + creationoffset;
			in_newbody = mPhysicsMgr->CreateBody(&bodydefinition,in_bodyname.str());
			assert(in_newbody);
			mPhysicsMgr->CreateCircleShape(&circledefinition,in_bodyname.str());
			in_newbody->SetMassFromShapes();
			totalblobmass += in_newbody->GetMass();
			//Assign user data pointer
			in_newbody->SetUserData(mRelatedAgent);
		}

		//*****Creation of spring joints*****
		//Outer spring joints
		//IF - There was a previous body created
		if(prevbody || ( creationparams.doubleskinned && prevbody && in_prevbody ))
		{
			std::stringstream jointname;
			jointname<<"Blob"<<mBlobsCreated<<"Joint"<<bodyname.str()<<prevbodyname;
			mPhysicsMgr->CreateDistanceJoint(&skinspringdef,  //Definition for skin spring joint
											 jointname.str(),  //joint name
											 prevbodyname,     //body name 1 (previous outer mass)
											 bodyname.str(),   //body name 2 (outer mass)
											 prevbody->GetPosition(), //body 1 position (previous outer mass)
											 newbody->GetPosition()); //body 2 position (outer mass)
			//Add joint to controller
			mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));
			//Inner skin joints creation
			if(creationparams.doubleskinned)
			{
				std::stringstream in_jointname;
				in_jointname<<"Blob"<<mBlobsCreated<<"Joint"<<in_bodyname.str()<<in_prevbodyname;
				mPhysicsMgr->CreateDistanceJoint(&skinspringdef,  //Definition for skin spring joint
												 in_jointname.str(),  //joint name
												 in_prevbodyname,     //body name 1 (previous inner mass)
												 in_bodyname.str(),   //body name 2 (inner mass)
												 in_prevbody->GetPosition(), //body 1 position (previous inner mass)
												 in_newbody->GetPosition()); //body 2 position (inner mass)				

				//Add joint to controller
				mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(in_jointname.str())));
			}
		}//ELSE - There was no previous body created
		else
		{
			//Just update values for last iteration (out of loop)
			firstbody = newbody;
			firstbodyname = bodyname.str();
			bodyname.clear();
			//Inner skin (if selected)
			if(creationparams.doubleskinned)
			{
				in_firstbody = in_newbody;
				in_firstbodyname = in_bodyname.str();
				in_bodyname.clear();
			}
		}
		
		//Spring joints to center mass
		//IF - Simple skin
		if(!creationparams.doubleskinned)
		{
			//Simple joint from outer skin to center
			std::stringstream jointname;
			jointname<<"Blob"<<mBlobsCreated<<"Joint"<<bodyname.str()<<innerbodyname.str();
			mPhysicsMgr->CreateDistanceJoint(&springdef,
											 jointname.str(),
											 innerbodyname.str(),
											 bodyname.str(),
											 centerbody->GetPosition(),
											 newbody->GetPosition());
			//Add joint to controller
			mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));

		}//ELSE - Double skin
		else
		{
			//Three joints : From outer skin to inner skin, from inner skin to center, and crossed interskin
			//Interskinjoint
			std::stringstream jointname;
			jointname<<"Blob"<<mBlobsCreated<<"Joint"<<bodyname.str()<<in_bodyname.str();
			mPhysicsMgr->CreateDistanceJoint(&springdef,
											 jointname.str(),
											 in_bodyname.str(),
											 bodyname.str(),
											 in_newbody->GetPosition(),
											 newbody->GetPosition());

			//Add joint to controller
				mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));

			//Crossed interskinjoint
			if(prevbody)
			{
				jointname.clear();
				jointname<<"Blob"<<mBlobsCreated<<"Joint"<<prevbodyname<<in_bodyname.str();
				mPhysicsMgr->CreateDistanceJoint(&incrossedspringdef,
												 jointname.str(),
												 prevbodyname,
												 in_bodyname.str(),
												 prevbody->GetPosition(),
												 in_newbody->GetPosition());
				//Add joint to controller
				mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));
			}
			//Innerskin - center joint
			jointname.clear();
			jointname<<"Blob"<<mBlobsCreated<<"Joint"<<in_bodyname.str()<<innerbodyname.str();
			mPhysicsMgr->CreateDistanceJoint(&inskinspringdef,
											 jointname.str(),
											 innerbodyname.str(),
											 in_bodyname.str(),
											 centerbody->GetPosition(),
											 in_newbody->GetPosition());
			//Add joint to controller
			mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));
		}
		//*****Update created bodies to blob controller*****
		if(newbody)
			mBlobControllerptr->AddBodyToControl(newbody);
		if(in_newbody)
			mBlobControllerptr->AddBodyToControl(in_newbody);
		//*****Update creation rotation for next mass*****
		creationrotation = b2Mul(rotationmatrix,creationrotation);
		if(creationparams.doubleskinned)
			innercreationrotation = b2Mul(rotationmatrix,innercreationrotation);
		//Update previous body data
		prevbody = newbody;
		prevbodyname = bodyname.str();
		if(creationparams.doubleskinned)
		{
			in_prevbody = in_newbody;
			in_prevbodyname = in_bodyname.str();
		}
	}//LOOP END


	//*****Make final connection between last and first body*****
	assert(firstbody);
	std::stringstream jointname;
	jointname<<"Blob"<<mBlobsCreated<<"FinalJoint";
	mPhysicsMgr->CreateDistanceJoint(&skinspringdef,
									 jointname.str(),
									 firstbodyname,
									 bodyname.str(),
									 firstbody->GetPosition(),
									 newbody->GetPosition());	
	//Add joint to controller
				mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));
	//IF - Double skin
	if(creationparams.doubleskinned)
	{
		//Internal skin final joint
		assert(in_firstbody);
		std::stringstream jointname;
		jointname<<"Blob"<<mBlobsCreated<<"InnerFinalJoint";
		mPhysicsMgr->CreateDistanceJoint(&inskinspringdef,
										 jointname.str(),
										 in_firstbodyname,
										 in_bodyname.str(),
										 in_firstbody->GetPosition(),
										 in_newbody->GetPosition());
		//Add joint to controller
				mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));

		//Crossed interskin final joint
		jointname.clear();
		jointname<<"Blob"<<mBlobsCreated<<"Joint"<<in_firstbodyname<<bodyname.str();
		mPhysicsMgr->CreateDistanceJoint(&incrossedspringdef,
										 jointname.str(),
										 bodyname.str(),
										 in_firstbodyname,
										 newbody->GetPosition(),
										 in_firstbody->GetPosition());

		//Add joint to controller
		mBlobControllerptr->AddJointToList(static_cast<b2DistanceJoint*>(mPhysicsMgr->GetJoint(jointname.str())));
	}

	/*//Create a sensor shape for character control processing only
	b2CircleDef boundingsensordef;
	//TODO: MERGE WITH GROUPS DEFINITIONS IN OVERALL SYSTEM!
	boundingsensordef.filter.groupIndex = 2;
	boundingsensordef.filter.categoryBits = 0x04;
	boundingsensordef.filter.maskBits = 0x01;
	boundingsensordef.isSensor = true;
	boundingsensordef.radius = creationparams.radius + creationparams.massesradius; // Radius (max radius)	
	mPhysicsMgr->CreateCircleShape(&boundingsensordef,innerbodyname.str());*/
	
	//Update blob controller with changes, last body (inner)
	mBlobControllerptr->SetCenterBody(centerbody);
	mBlobControllerptr->SetTotalMass(totalblobmass);
	//*********************BLOB CREATED!!**************************************

	mBlobsCreated++;

	SingletonLogMgr::Instance()->AddNewLine("BlobBuilder::LoadBlob","New blob created!",LOGDEBUG);
}