#pragma once

namespace VEngine::Graphics
{
	class Camera
	{
	public:
		enum class ProjectionMode
		{
			Perspective,
			Orthographic
		};

		void SetMode(ProjectionMode mode);

		//external settings
		void SetPosition(const Math::Vector3& position);
		void SetDirection(const Math::Vector3& direction);
		void SetLookAt(const Math::Vector3& target);

		//internal settings
		void SetFOV(float fov);
		void SetNearPlane(float nearPlane);
		void SetFarPlane(float farPlane);

		//mode specfic
		//projection
		void SetAspectRatio(float ratio);
		//orthographic
		void SetSize(float width, float height);
		
		//movement controls
		void Walk(float distance);
		void Strafe(float distance);
		void Rise(float distance);
		void Yaw(float radians);
		void Pitch(float radians);
		void Zoom(float amount);

		//return values
		const Math::Vector3& GetPosition() const;
		const Math::Vector3& GetDirection() const;

		//graphics pipeline values
		Math::Matrix4 GetViewMatrix() const;
		Math::Matrix4 GetProjectionMatrix() const;

		//different projection matrices for each mode
		Math::Matrix4 GetPerspectiveMatrix() const;
		Math::Matrix4 GetOrthographicMatrix() const;
	private:
		ProjectionMode mMode = ProjectionMode::Perspective;
		Math::Vector3 mPosition = Math::Vector3::Zero;
		Math::Vector3 mDirection = Math::Vector3::ZAxis;

		float mFov = 60.0f * Math::Constants::DegToRad;
		float mNearPlane = 0.01f;
		float mFarPlane = 10000.0f;

		float mAspcetRatio = 0.0f;
		float mWidth = 0.0f;
		float mHeight = 0.0f;
	};
}