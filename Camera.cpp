#include "Camera.h"

using namespace DirectX;

Camera::Camera(float aspectRatio, XMFLOAT3 initialPosition, XMFLOAT3 startingOrientation,float fov, float nearClipPlane, float farClipPlane, float movementSpeed, float mouseLookSpeed)
{
    // Setup the Cameras Transform
    transform = Transform();
    transform.SetPosition(initialPosition);
    transform.SetRotation(startingOrientation);

    // Setup the View and Projection Matrices
    UpdateProjectionMatrix(aspectRatio);
    UpdateViewMatrix();

    // Setup customizable values
    fovAngle = fov;
    this->nearClipPlane = nearClipPlane;
    this->farClipPlane = farClipPlane;
    this->movementSpeed = movementSpeed;
    this->mouseLookSpeed = mouseLookSpeed;
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
    return projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    XMMATRIX currentProjectionMatrix = XMMatrixPerspectiveFovLH(fovAngle, aspectRatio, nearClipPlane, farClipPlane);
    XMStoreFloat4x4(&projectionMatrix, currentProjectionMatrix);
}

void Camera::UpdateViewMatrix()
{
    XMMATRIX currentViewMatrix = XMMatrixLookToLH(XMLoadFloat3(&transform.GetPosition()), XMLoadFloat3(&transform.GetForward()), XMLoadFloat3(&transform.GetUp()));
    XMStoreFloat4x4(&viewMatrix, currentViewMatrix);
}

void Camera::Update(float deltaTime)
{
    // Speed Management
    float speed = (movementSpeed * deltaTime) * 5;

    // Directional movement management
    if (Input::KeyDown('W')) { transform.MoveRelative(0, 0, +speed); }
    if (Input::KeyDown('S')) { transform.MoveRelative(0, 0, -speed); }
    if (Input::KeyDown('A')) { transform.MoveRelative(-speed, 0, 0); }
    if (Input::KeyDown('D')) { transform.MoveRelative(+speed, 0, 0); }
    if (Input::KeyDown(VK_SHIFT)) { transform.MoveAbsolute(0, -speed, 0); }
    if (Input::KeyDown(VK_CONTROL)) { transform.MoveAbsolute(0, +speed, 0); }

    // Mouse movement management
    if (Input::MouseLeftDown()) 
    {   
        // Get the new rotation
        int cursorMovementX = Input::GetMouseXDelta();
        int cursorMovementY = Input::GetMouseYDelta();

        float dx = cursorMovementX * mouseLookSpeed;
        float dy = cursorMovementY * mouseLookSpeed;

        transform.Rotate(dy, dx, 0);

        // Clamp the values of the new rotation
        XMFLOAT3 clampingRotation = transform.GetPitchYawRoll();
        float xValue = clampingRotation.x;
        if (xValue > XM_PIDIV2) xValue = XM_PIDIV2;
        else if (xValue < -XM_PIDIV2) xValue = -XM_PIDIV2;
        clampingRotation.x = xValue;
        transform.SetRotation(clampingRotation);
    }

    // Update the view matrix
    UpdateViewMatrix();
}
