#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"

// Add the ImGui header files
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Initialize all ImGui variables
	backgroundColor = { 0.4f, 0.6f, 0.75f, 1.0f };
	demoVisible = false;
	menuStyle = 1;
	colorTints.push_back({ 1.0f, 0.75f, 0.13f, 1.0f});
	colorTints.push_back({ 0.45f, 1.0f, 0.26f, 1.0f });
	colorTints.push_back({ 0.32f, 0.81f, 1.0f, 1.0f });
	offset = { 1.0f, 0.0f, 0.0f };

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();
	
	// Tell the input assembler (IA) stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our vertices?"
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Ensure the pipeline knows how to interpret all the numbers stored in
	// the vertex buffer. For this course, all of your vertices will probably
	// have the same layout, so we can just set this once at startup.
	Graphics::Context->IASetInputLayout(inputLayout.Get());

	// Setup all Constant Buffers
	// Calculate the next biggest multiple of 16
	int size = sizeof(VSExternalData);
	size = (size + 15) / 16 * 16;

	D3D11_BUFFER_DESC vsCBDesc{};
	vsCBDesc.Usage = D3D11_USAGE_DYNAMIC;
	vsCBDesc.ByteWidth = size;
	vsCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vsCBDesc.MiscFlags = 0;
	vsCBDesc.StructureByteStride = 0;

	Graphics::Device->CreateBuffer(&vsCBDesc, 0, VSConstantBuffer.GetAddressOf());
	// Bind the constant buffer
	// to the proper stage of the rendering pipeline
	Graphics::Context->VSSetConstantBuffers(
		0, // Must match cbuffer register in shader!!!!
		1, // The number of buffers we are setting
		VSConstantBuffer.GetAddressOf()); // The array of buffers (or just the one we have)
		
	size = sizeof(PSExternalData);
	size = (size + 15) / 16 * 16;

	D3D11_BUFFER_DESC psCBDesc{};
	psCBDesc.Usage = D3D11_USAGE_DYNAMIC;
	psCBDesc.ByteWidth = size;
	psCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	psCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	psCBDesc.MiscFlags = 0;
	psCBDesc.StructureByteStride = 0;

	Graphics::Device->CreateBuffer(&psCBDesc, 0, PSConstantBuffer.GetAddressOf());

	// Bind the constant buffer
	// to the proper stage of the rendering pipeline
	Graphics::Context->PSSetConstantBuffers(
		0, // Must match cbuffer register in shader!!!!
		1, // The number of buffers we are setting
		PSConstantBuffer.GetAddressOf()); // The array of buffers (or just the one we have)

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	ImGui::StyleColorsDark();

	// Initialize the Camera
	cameras.push_back(std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(0.0f, +2.0f, -20.0f),
		45 * (XM_PI / 180), 0.01f, 100.0f, 5.0f, 5.0f));
	cameras.push_back(std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(-0.5f, 0.0f, -10.0f),
		90 * (XM_PI / 180), 0.01f, 100.0f, 5.0f, 5.0f));
	activeCamera = 0;
}

// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	// Load the only Vertex Shader
	ID3DBlob* vertexShaderBlob;

	// Read our compiled shader code files into blobs
	// - Essentially just "open the file and plop its contents here"
	// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
	// - Note the "L" before the string - this tells the compiler the string uses wide characters
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

	// Create the actual Direct3D shaders on the GPU
	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		vertexShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer

	// Load the Pixel Shaders via a helper method to make it easier
	pixelShader = LoadPixelShader(FixPath(L"PixelShader.cso").c_str());
	pixelShaderUV = LoadPixelShader(FixPath(L"DebugUVsPS.cso").c_str());
	pixelShaderNormal = LoadPixelShader(FixPath(L"DebugNormalsPS.cso").c_str());
	pixelShaderCustom = LoadPixelShader(FixPath(L"CustomPS.cso").c_str());

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	D3D11_INPUT_ELEMENT_DESC inputElements[3] = {};

	// Set up the first element - a position, which is 3 float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
	inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

	// Set up the second element - uv values, which is 2 more float values
	inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;					// 2x 32-bit floats
	inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input!
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

	// Set up the third element - normal values, which is 3 more float values
	inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// 3x 32-bit floats
	inputElements[2].SemanticName = "NORMAL";							// Match our vertex shader input!
	inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

	// Create the input layout, verifying our description against actual shader code
	Graphics::Device->CreateInputLayout(
		inputElements,							// An array of descriptions
		3,										// How many elements in that array?
		vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
		vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
		inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create the materials that we will be using
	materials.push_back(std::make_shared<Material>("Red Tint", colorTints[0], vertexShader, pixelShader));
	materials.push_back(std::make_shared<Material>("Green Tint", colorTints[1], vertexShader, pixelShader));
	materials.push_back(std::make_shared<Material>("Blue Tint", colorTints[2], vertexShader, pixelShader));
	materials.push_back(std::make_shared<Material>("UV Values", colorTints[0], vertexShader, pixelShaderUV));
	materials.push_back(std::make_shared<Material>("Normal Values", colorTints[0], vertexShader, pixelShaderNormal));
	materials.push_back(std::make_shared<Material>("Custom", colorTints[0], vertexShader, pixelShaderCustom));

	// Create the various meshes that we need
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/torus.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad_double_sided.obj").c_str()));

	// push back all of the premade and adjusted gameEntities
	gameObjects.push_back(std::make_shared<GameEntities>("Cube", meshes[0], materials[0]));
	gameObjects.push_back(std::make_shared<GameEntities>("Sphere", meshes[1], materials[1]));
	gameObjects.push_back(std::make_shared<GameEntities>("Cylinder", meshes[2], materials[2]));
	gameObjects.push_back(std::make_shared<GameEntities>("Helix", meshes[3], materials[5]));
	gameObjects.push_back(std::make_shared<GameEntities>("Torus", meshes[4], materials[2]));
	gameObjects.push_back(std::make_shared<GameEntities>("Quad", meshes[5], materials[1]));
	gameObjects.push_back(std::make_shared<GameEntities>("Quad Double Sided", meshes[6], materials[0]));

	// Adjust positions
	gameObjects[0]->GetTransform()->MoveAbsolute(-9.0f, 0.0f, 0.0f);
	gameObjects[1]->GetTransform()->MoveAbsolute(-6.0f, 0.0f, 0.0f);
	gameObjects[2]->GetTransform()->MoveAbsolute(-3.0f, 0.0f, 0.0f);
	gameObjects[4]->GetTransform()->MoveAbsolute(+3.0f, 0.0f, 0.0f);
	gameObjects[5]->GetTransform()->MoveAbsolute(+6.0f, 0.0f, 0.0f);
	gameObjects[6]->GetTransform()->MoveAbsolute(+9.0f, 0.0f, 0.0f);

	// Create UV gameobjects
	gameObjects.push_back(std::make_shared<GameEntities>("UV Cube", meshes[0], materials[3]));
	gameObjects.push_back(std::make_shared<GameEntities>("UV Sphere", meshes[1], materials[3]));
	gameObjects.push_back(std::make_shared<GameEntities>("UV Cylinder", meshes[2], materials[3]));
	gameObjects.push_back(std::make_shared<GameEntities>("UV Helix", meshes[3], materials[3]));
	gameObjects.push_back(std::make_shared<GameEntities>("UV Torus", meshes[4], materials[3]));
	gameObjects.push_back(std::make_shared<GameEntities>("UV Quad", meshes[5], materials[3]));
	gameObjects.push_back(std::make_shared<GameEntities>("UV Quad Double Sided", meshes[6], materials[3]));

	// Adjust positions
	gameObjects[7]->GetTransform()->MoveAbsolute(-9.0f, +3.0f, 0.0f);
	gameObjects[8]->GetTransform()->MoveAbsolute(-6.0f, +3.0f, 0.0f);
	gameObjects[9]->GetTransform()->MoveAbsolute(-3.0f, +3.0f, 0.0f);
	gameObjects[10]->GetTransform()->MoveAbsolute(0.0f, +3.0f, 0.0f);
	gameObjects[11]->GetTransform()->MoveAbsolute(+3.0f, +3.0f, 0.0f);
	gameObjects[12]->GetTransform()->MoveAbsolute(+6.0f, +3.0f, 0.0f);
	gameObjects[13]->GetTransform()->MoveAbsolute(+9.0f, +3.0f, 0.0f);

	// Create Normal Value gameobjects
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Cube", meshes[0], materials[4]));
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Sphere", meshes[1], materials[4]));
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Cylinder", meshes[2], materials[4]));
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Helix", meshes[3], materials[4]));
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Torus", meshes[4], materials[4]));
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Quad", meshes[5], materials[4]));
	gameObjects.push_back(std::make_shared<GameEntities>("Normal Quad Double Sided", meshes[6], materials[4]));

	// Adjust positions
	gameObjects[14]->GetTransform()->MoveAbsolute(-9.0f, +6.0f, 0.0f);
	gameObjects[15]->GetTransform()->MoveAbsolute(-6.0f, +6.0f, 0.0f);
	gameObjects[16]->GetTransform()->MoveAbsolute(-3.0f, +6.0f, 0.0f);
	gameObjects[17]->GetTransform()->MoveAbsolute(0.0f, +6.0f, 0.0f);
	gameObjects[18]->GetTransform()->MoveAbsolute(+3.0f, +6.0f, 0.0f);
	gameObjects[19]->GetTransform()->MoveAbsolute(+6.0f, +6.0f, 0.0f);
	gameObjects[20]->GetTransform()->MoveAbsolute(+9.0f, +6.0f, 0.0f);
}

// Get the ImGui library all the information that it needs to be created
void Game::ImGuiCreate(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Determine new input capture 
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// Create a custom ImGui Debug menu
	ImGui::Begin("Inspector Menu");
	{
		// Create a header for the Debuggin options
		if (ImGui::CollapsingHeader("Debuging")) {
			// Framerate Tracking
			// Replace the %f with the next parameter, and format as a float
			ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);

			// Window Size
			// Replace each %d with the next parameter, and format as decimal integers
			// The "x" will be printed as-is between the numbers, like so: 800x600
			ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());

			// Color Picker for selecting different colors for the background
			ImGui::ColorEdit4("Background Color", &backgroundColor.x);

			// Button for displaying the demo menu when true
			if (ImGui::Button("Show Demo Window"))
				demoVisible = !demoVisible;

			if (demoVisible)
				ImGui::ShowDemoWindow();

			// Unique Element 1: A Button that when pressed changes the menu style
			if (ImGui::Button("Change Style"))
			{
				if (menuStyle == 3) {
					menuStyle = 1;
				}
				else {
					menuStyle++;
				}
			}

			switch (menuStyle) {
			case 1:
				ImGui::StyleColorsDark();
				break;
			case 2:
				ImGui::StyleColorsLight();
				break;
			case 3:
				ImGui::StyleColorsClassic();
				break;
			default:
				ImGui::StyleColorsDark();
			}
		}

		//// Collapsing header for changing the tint and offset of the meshes
		//if (ImGui::CollapsingHeader("Vertex Shaders")) 
		//{
		//	ImGui::ColorEdit4("Color Tint", &colorTints[0].x);
		//	ImGui::SliderFloat3("Mesh Offset", &offset.x, -1.0f, 1.0f);
		//}

		// Create a collapsing header for the purpose of viewing Mesh information
		if (ImGui::CollapsingHeader("3D Mesh Details")) {
			// Display the information for each individual mesh
			unsigned int index = 0;
			for (std::shared_ptr<GameEntities> entity : gameObjects) {
				ImGui::PushID(index);
				std::shared_ptr<Mesh> entityMesh = entity->GetMesh();
				if (ImGui::TreeNode(entity->GetName())) {
					ImGui::Text("Triangles: %i", (entityMesh->GetIndexCount() / 3));
					ImGui::Text("Vertices: %i", entityMesh->GetVertexCount());
					ImGui::Text("Indices: %i", entityMesh->GetIndexCount());
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}

		// Create a collapsing header for the purpose of viewing Mesh information
		if (ImGui::CollapsingHeader("Entity Transforms")) {
			// Display the information for each individual mesh
			unsigned int index = 0;
			for (std::shared_ptr<GameEntities> entity : gameObjects) {
				ImGui::PushID(index);
				if (ImGui::TreeNode(entity->GetName())) {
					// Get access to the required information
					XMFLOAT3 tempPosition = entity->GetTransform()->GetPosition();
					XMFLOAT3 tempRotation = entity->GetTransform()->GetPitchYawRoll();
					XMFLOAT3 tempScale = entity->GetTransform()->GetScale();
					// Create the proper dials for those values
					ImGui::DragFloat3("Position", &tempPosition.x, 0.01f);
					ImGui::DragFloat3("Rotation(Radians)", &tempRotation.x, 0.01f);
					ImGui::DragFloat3("Scale", &tempScale.x, 0.01f);
					// Set the new values
					entity->GetTransform()->SetPosition(tempPosition);
					entity->GetTransform()->SetRotation(tempRotation);
					entity->GetTransform()->SetScale(tempScale);
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}

		// Create a collapsing header for the purpose of viewing Camera Information
		if (ImGui::CollapsingHeader("Cameras")) {
			// Radio Buttons for camera selection
			ImGui::RadioButton("Camera 1", &activeCamera, 0); ImGui::SameLine();
			ImGui::RadioButton("Camera 2", &activeCamera, 1);
			// Add some basic information that can be changed in the debug menu
			XMFLOAT3 tempPosition = cameras[activeCamera]->GetTransform()->GetPosition();
			XMFLOAT3 tempRotation = cameras[activeCamera]->GetTransform()->GetPitchYawRoll();
			ImGui::DragFloat3("Position", &tempPosition.x, 0.01f);
			ImGui::DragFloat3("Rotation(Radians)", &tempRotation.x, 0.01f);
			cameras[activeCamera]->GetTransform()->SetPosition(tempPosition);
			cameras[activeCamera]->GetTransform()->SetRotation(tempRotation);
			// Add some non-changing information to the menu
			ImGui::Text("FOV: %f radians", cameras[activeCamera]->GetFov());
			ImGui::Text("Near Clip Plane: %f", cameras[activeCamera]->GetNearClipPlane());
			ImGui::Text("Far Clip Plane: %f", cameras[activeCamera]->GetFarClipPlane());
		}
	}
	ImGui::End();
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Game::LoadPixelShader(const wchar_t* filePath)
{
	ID3DBlob* tempShaderBlob;
	D3DReadFileToBlob(filePath, &tempShaderBlob);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> tempShader;
	Graphics::Device->CreatePixelShader(
		tempShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		tempShaderBlob->GetBufferSize(),	// How big is that data?
		0,									// No classes in this shader
		tempShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

	return tempShader;
}

// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Resize the projection matrix
	for (std::shared_ptr<Camera> camera : cameras) 
	{
		if (camera != nullptr)
			camera->UpdateProjectionMatrix(Window::AspectRatio());
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Inform the ImGui of the relevant data
	Game::ImGuiCreate(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	for (std::shared_ptr<GameEntities> entity : gameObjects) {
		entity->GetTransform()->Rotate(0.0f, deltaTime, 0.0f);
	}

	float scaleValue = (float)sin(totalTime) * 0.25f + 1.0f;
	gameObjects[0]->GetTransform()->SetScale(scaleValue, scaleValue, scaleValue);
	gameObjects[7]->GetTransform()->SetScale(scaleValue, scaleValue, scaleValue);
	gameObjects[14]->GetTransform()->SetScale(scaleValue, scaleValue, scaleValue);

	cameras[activeCamera]->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	// Clear the back buffer (erase what's on screen) and depth buffer
	Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	&backgroundColor.x);
	Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	for(std::shared_ptr<GameEntities> gameEntity : gameObjects) {
		// Set the vertex shader and pixel shader
		Graphics::Context->VSSetShader(gameEntity->GetMaterial()->GetVertexShader().Get(), 0, 0);
		Graphics::Context->PSSetShader(gameEntity->GetMaterial()->GetPixelShader().Get(), 0, 0);

		// Set the data for the Vertex Shader constant buffer
		VSExternalData vsData{};
		vsData.World = gameEntity->GetTransform()->GetWorldMatrix();
		vsData.ViewMatrix = cameras[activeCamera]->GetViewMatrix();
		vsData.ProjectionMatrix = cameras[activeCamera]->GetProjectionMatrix();

		// Copy the above struct data directly into GPU memory
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		Graphics::Context->Map(
			VSConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedBuffer);
		memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
		Graphics::Context->Unmap(VSConstantBuffer.Get(), 0);

		// Set the data for the Pixel Shader constant buffer
		PSExternalData psData{};
		psData.ColorTint = gameEntity->GetMaterial()->GetColorTint();
		psData.TotalTime = totalTime;

		// Copy the above struct data directly into GPU memory
		Graphics::Context->Map(
			PSConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedBuffer);
		memcpy(mappedBuffer.pData, &psData, sizeof(psData));
		Graphics::Context->Unmap(PSConstantBuffer.Get(), 0);
		
		// Finally draw the item
		gameEntity->Draw();
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	ImGui::Render(); // Truns this frame's UI into rednerable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

	// Present at the end of the frame
	bool vsync = Graphics::VsyncState();
	Graphics::SwapChain->Present(
		vsync ? 1 : 0,
		vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

	// Re-bind back buffer and depth buffer after presenting
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());
}