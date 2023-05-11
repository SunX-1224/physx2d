#include "PhysicsSimulator.h"

std::map<int, Shader> PhysicsSimulator::Shaders;

std::map<int, std::vector<PhysicsBody>> PhysicsSimulator::Objects;
std::map<int, InstancedRenderer> PhysicsSimulator::renderers;
std::map<int, std::vector<SpriteRenderer>> PhysicsSimulator::spriteRenderers;
double PhysicsSimulator::last_time;

Window PhysicsSimulator::AppWindow;

void PhysicsSimulator::Init(){
	//window init
	AppWindow.Init();

	//shaders init
	LoadShader("SandBox/res/vert.glsl", "SandBox/res/circle.glsl", Type::CIRCLE);
	LoadShader("SandBox/res/vert.glsl", "SandBox/res/quad.glsl", Type::QUAD);
	LoadShader("SandBox/res/frame_vs.glsl", "SandBox/res/frame_fs.glsl", 2);

	PhysicsBody* constraint = AddObject(Type::BoxBound);
	constraint->transform->Scale = vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
	((AxisAlignedBB*)(constraint->collider))->Size = constraint->transform->Scale;
	constraint->spriteRenderer->color = vec4(0.2f);

#if 1
	for (int i = 0; i < 2500; i++) {
		vec2 randomPos = vec2(randint(-SCREEN_WIDTH, SCREEN_WIDTH) * 0.5f, randint(-SCREEN_HEIGHT, SCREEN_HEIGHT) * 0.5f);
		float mass = 7.f;

		PhysicsBody* body = AddObject(Type::CIRCLE);
		body->transform->Position = randomPos;
		body->transform->Scale = vec2(mass);
		((CircleCollider*)(body->collider))->Radius = body->transform->Scale.x * 0.5f;
		body->rigidbody2d->mass = mass;
		body->rigidbody2d->Type = DYNAMIC;
		body->rigidbody2d->coef_restitution = 0.8f;
		body->rigidbody2d->coef_drag = 0.2f;
		body->spriteRenderer->color = vec4(random(i), 0.2f, random(i), 1.0f);
		//body->rigidbody2d->Velocity = vec2(random(i) * 100.f, random(i+1)*100.f);
	}
#endif
}

void PhysicsSimulator::Run(){

	Time::initTimer();
	last_time = 0.0f;
	while (!AppWindow.ShouldClose()) {
		//Input handler
		AppWindow.UpdateEvents();

		ProcessInput();

		//Update Handler
#if 0
		if (TIME - last_time > .05f){
			last_time = TIME;

			float mass = 5.f;
			PhysicsBody* body = AddObject(Type::CIRCLE);

			body->transform->Position = vec2(0.);
			body->transform->Scale = vec2(mass);
			((CircleCollider*)(body->collider))->Radius = body->transform->Scale.x * 0.5f;
			body->rigidbody2d->mass = mass;
			body->rigidbody2d->Type = DYNAMIC;
			body->rigidbody2d->Velocity = vec2(250.f, -50.f);
			body->rigidbody2d->coef_restitution = 0.8f;
			body->rigidbody2d->coef_drag = 0.7f;
			body->spriteRenderer->color = vec4(random(24), 0.2f, random(12), 1.0f);
		}
#endif
		Update(Time::get_delta_time());
		UpdateRenderData();

		//Rendering
		AppWindow.FillScreen(Color(0.f));
		Draw();

		AppWindow.Update();

		Time::update();
		AppWindow.SetTitle((std::to_string(Time::get_fps()) + " " + std::to_string(Objects[0].size())).c_str());
	}
}

void PhysicsSimulator::Draw(){
	//frame1.Bind();
	//frame1.Clear();
	renderers[Type::CircleBound].Draw(Shaders[Type::CIRCLE]);
	renderers[Type::BoxBound].Draw(Shaders[Type::QUAD]);
	renderers[Type::BoxBound].Draw(Shaders[Type::QUAD]);
	renderers[Type::CIRCLE].Draw(Shaders[Type::CIRCLE]);
	//frame1.Unbind();
	//frame1.ActivateAsTexture();
	//frame1.Draw(Shaders[2]);
}

void PhysicsSimulator::Clear(){
	AppWindow.Destroy();
	for (auto& shader : Shaders) {
		shader.second.del();
	}
	for (auto& renderer : renderers) {
		renderer.second.del();
	}

	for (auto& iter : Objects) {
		for (auto& object : iter.second) {
			object.del();
		}
	}

	//frame1.del();
}

PhysicsBody* PhysicsSimulator::AddObject(int type){
	bool createNew = true;
	for (auto& iter : renderers) {
		if (iter.first == type) {
			createNew = false;
			break;
		}
	}

	
	Collider* collider;

	if (type == Type::CIRCLE)
		collider = new CircleCollider;
	else if (type == Type::QUAD)
		collider = new BoxCollider2D;
	else if (type == Type::BoxBound)
		collider = new AxisAlignedBB;
	else if (type == Type::CircleBound)
		collider = new BoundingCircle;
	else 
		throw("Invalid type attempted to be created");
	
	Transform* transform = new Transform;
	RigidBody2D* rigidbody2d = new RigidBody2D;
	SpriteRenderer* spriteRenderer = new SpriteRenderer;

	PhysicsBody object(transform, rigidbody2d, collider, spriteRenderer);

	Objects[type].push_back(object);
	spriteRenderers[type].push_back(*object.spriteRenderer);

	if (createNew) {
		renderers[type] = InstancedRenderer(initVectorFromArray(QUAD_VERTICES, vec2), initVectorFromArray(QUAD_INDICES, uint32_t));
		renderers[type].Layout(0, 2, GL_FLOAT, sizeof(vec2), 0);
		renderers[type].InstanceLayout(1, 2, GL_FLOAT, sizeof(SpriteRenderer), 0); //vec2 position
		renderers[type].InstanceLayout(2, 2, GL_FLOAT, sizeof(SpriteRenderer), 2 * sizeof(float)); //vec2 scale
		renderers[type].InstanceLayout(3, 1, GL_FLOAT, sizeof(SpriteRenderer), 4 * sizeof(float)); // float rotation
		renderers[type].InstanceLayout(4, 4, GL_FLOAT, sizeof(SpriteRenderer), 5 * sizeof(float)); //vec4 color
	}
	return &Objects[type].back();
}

void PhysicsSimulator::Update(float delta_time){
	QuadTree<PhysicsBody*> qtree(centerRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT));

	for (auto& obj : Objects[Type::CIRCLE])
		qtree.insert(obj.transform->Position, &obj);

	for (int i = 0; i < Objects[Type::CIRCLE].size(); i++) {
		PhysicsBody& object = Objects[Type::CIRCLE][i];
		object.spriteRenderer->color = vec4(.0f, 0.1f, .7f, 0.5f);

		std::vector<PhysicsBody*> neighbours;
		qtree.query(centerRect(object.transform->Position.x, object.transform->Position.y, 15.f, 15.f), neighbours);

		for (int j = 0; j < neighbours.size(); j++) {

			if (&object == neighbours[j]) continue;

			CollisionData data = object.collider->checkCollision(neighbours[j]->collider);
			if (data.status) {
				collisionPhysics(data, object.rigidbody2d, neighbours[j]->rigidbody2d);
				object.spriteRenderer->color += vec4(0.1f, 0.0f, -0.1f, 0.5f);
				neighbours[j]->spriteRenderer->color += vec4(0.1f, 0.0f, -0.1f, 0.5f);
			}

			vec2 dis = neighbours[j]->transform->Position - object.transform->Position;
			vec2 force = dis.normalized() * (10000.f / dot(dis, dis));

			object.rigidbody2d->addForce(force);
			neighbours[j]->rigidbody2d->addForce(-force);
		}
		
		object.rigidbody2d->addAcceleration((-object.transform->Position).normalized()  * 50.f);
		/*CollisionData data = object.collider->constraintCollision(Objects[Type::BoxBound][0].collider);
		if (data.status)
			collisionPhysics(data, object.rigidbody2d, Objects[Type::BoxBound][0].rigidbody2d);*/

		updatePhysics(object.rigidbody2d, delta_time);
		object.spriteRenderer->transform = *object.transform;
		spriteRenderers[Type::CIRCLE][i] = *object.spriteRenderer;
	}

	//Objects[Type::BoxBound][0].spriteRenderer->transform = *Objects[Type::BoxBound][0].transform;
	//spriteRenderers[Type::BoxBound][0] = *Objects[Type::BoxBound][0].spriteRenderer;
}

void PhysicsSimulator::UpdateRenderData(){
	for (auto& iter : PhysicsSimulator::Shaders) {
		iter.second.use();
		iter.second.setVec2("u_resolution", AppWindow.GetResolution());
		iter.second.setMat3("u_ortho", get_ortho2d(vec2(0, 0), AppWindow.GetResolution()));
		iter.second.setFloat("u_time", Time::get_time());
	}

	Shaders[2].setInt("frame", 0);
	
	for (auto& iter : PhysicsSimulator::renderers) {
		iter.second.InstanceData(spriteRenderers[iter.first].data(), spriteRenderers[iter.first].size(), sizeof(SpriteRenderer));
	}
}

void PhysicsSimulator::ProcessInput(){
	if (glfwGetKey(AppWindow.m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		AppWindow.SetClose(true);

#if 1
	if (glfwGetMouseButton(AppWindow.m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		if (TIME - last_time < .05f) return;
		last_time = TIME;

		double x, y;
		glfwGetCursorPos(AppWindow.m_window, &x, &y);

		float mass = 5.f;
		vec2 res = AppWindow.GetResolution();
		
		PhysicsBody* body = AddObject(Type::CIRCLE);

		body->transform->Position = vec2(x - res.x*0.5f, res.y*0.5f-y);
		body->transform->Scale = vec2(mass);
		((CircleCollider*)(body->collider))->Radius = body->transform->Scale.x * 0.5f;
		body->rigidbody2d->mass = mass;
		body->rigidbody2d->Type = DYNAMIC;
		body->rigidbody2d->Velocity = -body->transform->Position;
		body->rigidbody2d->coef_restitution = 0.8f;
		body->rigidbody2d->coef_drag = 0.7f;
		body->spriteRenderer->color = vec4(random(24), 0.2f, random(12), 1.0f);
	}
#endif
}

void PhysicsSimulator::LoadShader(const char* vert, const char* frag, int name){
	Shaders[name] = Shader(vert, frag);
}
