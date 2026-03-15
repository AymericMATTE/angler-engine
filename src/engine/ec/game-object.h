#pragma once
#include <unordered_map>
#include "common/maths/transform.h"

namespace angler
{
	struct Component;

	class GameObject
	{
	public :

		GameObject* getParent();
		void setParent(GameObject* _parent);

		template<class T>
		T* addComponent();

		template<class T>
		T* getComponent();

		template<class T>
		bool hasComponent();

		template<class T>
		void removeComponent();

		void Enabled();
		void Disabled();

		bool isEnabled();

		void Start();

		void Update();
		void FixedUpdate();

		void Collide(GameObject* _other);

		void PreRender();
		void Render3D();
		void RenderUI();

		void Destroy();

		~GameObject();

		// Transform

		void setPosition(const DirectX::XMFLOAT3& _position);
		void setRotation(const DirectX::XMFLOAT4& _rotation);
		void setRotation(const DirectX::XMFLOAT3& _rotation);
		void setScale(const DirectX::XMFLOAT3& _scale);

		void translate(const DirectX::XMFLOAT3& _delta);
		void rotate(const DirectX::XMFLOAT4& _delta);
		void rotate(const DirectX::XMFLOAT3& _rotation);
		void scale(const DirectX::XMFLOAT3& _factor);

		void setLocalPosition(const DirectX::XMFLOAT3& _position);
		void setLocalRotation(const DirectX::XMFLOAT4& _rotation);
		void setLocalScale(const DirectX::XMFLOAT3& _scale);

		void localTranslate(const DirectX::XMFLOAT3& _delta);
		void localRotate(const DirectX::XMFLOAT4& _delta);
		void localRotate(const DirectX::XMFLOAT3& _delta);
		void localScale(const DirectX::XMFLOAT3& _factor);

		void lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& up = { 0.f, 1.f, 0.f });
		void lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& up = { 0.f, 1.f, 0.f });

		[[nodiscard]] DirectX::XMFLOAT3 getPosition();
		[[nodiscard]] DirectX::XMFLOAT4 getRotation();
		[[nodiscard]] DirectX::XMFLOAT3 getScale();

		[[nodiscard]] DirectX::XMFLOAT3 getLocalPosition();
		[[nodiscard]] DirectX::XMFLOAT4 getLocalRotation();
		[[nodiscard]] DirectX::XMFLOAT3 getLocalScale();

		[[nodiscard]] DirectX::XMFLOAT4X4 getWorldMatrix();

		[[nodiscard]] DirectX::XMFLOAT3 getRight();
		[[nodiscard]] DirectX::XMFLOAT3 getUp();
		[[nodiscard]] DirectX::XMFLOAT3 getForward();

	private :
		GameObject() = default;
		void removeChild(GameObject* _child);
		void addChild(GameObject* _child);
		void updateParent();

		unsigned int m_id = 0;
		int m_sceneId = 0;
		std::unordered_map<unsigned long long, Component*> m_components = {};

		bool m_toDestroy = false;
		bool m_isEnabled = true;

		Transform m_transform;

		GameObject* m_parent = nullptr;
		std::vector<GameObject*> m_childrens = {};

		int m_tick = 0;

		friend class Scene;
		friend class Application;
		friend class HGrid;
		friend class MeshComponent;
	};
}

#include "game-object.inl"

