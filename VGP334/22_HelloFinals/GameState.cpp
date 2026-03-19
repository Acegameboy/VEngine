#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

namespace
{
    float RandFloat(float lo, float hi)
    {
        return lo + static_cast<float>(rand()) / RAND_MAX * (hi - lo);
    }

    void PlayBeep(DWORD freq, DWORD ms)
    {
        struct Pair { DWORD f, m; };
        auto* p = new Pair{ freq, ms };
        HANDLE h = CreateThread(nullptr, 0,
            [](LPVOID arg) -> DWORD {
                auto* pp = reinterpret_cast<Pair*>(arg);
                Beep(pp->f, pp->m);
                delete pp;
                return 0;
            }, p, 0, nullptr);
        if (h) CloseHandle(h);
    }
} 
void GameState::Initialize()
{
    srand(42);

    mCamera.SetPosition({ 0.0f, 2.5f, -9.0f });
    mCamera.SetLookAt({ 0.0f, 1.0f, 0.0f });

    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.3f, 0.3f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.9f, 0.85f, 1.0f, 1.0f };
    mDirectionalLight.specular = { 1.0f, 1.0f,  1.0f, 1.0f };

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    ModelManager* mm = ModelManager::Get();

    mCharacter01.Initialize("Character01/Character01.model");
    mCharacter01.animator = &mAnimator01;

    mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Idle.animset");
    mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Running.animset");
    mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Jump.animset");
    mm->AddAnimation(mCharacter01.modelId, L"../../Assets/Models/Character01/Dying.animset");

    mAnimator01.Initialize(mCharacter01.modelId);
    mAnimator01.PlayAnimation(Char1Clip::Idle, true);   

    mCharacter01.transform.position = mChar1Pos;
    mCharacter01.transform.rotation =
        Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, mChar1Yaw);

    mCharacter02.Initialize("Character02/Character02.model");
    mCharacter02.animator = &mAnimator02;

    mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character02/Idle.animset");
    mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character02/Running.animset");
    mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character02/SillyDancing.animset");
    mm->AddAnimation(mCharacter02.modelId, L"../../Assets/Models/Character02/Jump.animset");

    mAnimator02.Initialize(mCharacter02.modelId);
    mAnimator02.PlayAnimation(Char2Clip::Idle, true);   

    mCharacter02.transform.position = mChar2Pos;
    mCharacter02.transform.rotation =
        Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, mChar2Yaw);

    mEvents.push_back({ 0.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Idle, true);
        mAnimator02.PlayAnimation(Char2Clip::Idle, true);
        PlayBeep(880, 120); PlayBeep(1100, 120); PlayBeep(1320, 200);
    } });

    mEvents.push_back({ 3.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Running, true);
        PlayBeep(660, 80);
    } });

    mEvents.push_back({ 5.0f, false, [this]() {
        mAnimator02.PlayAnimation(Char2Clip::Running, true);
        PlayBeep(880, 80);
    } });

    mEvents.push_back({ 10.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Jump, false);
        SpawnExplosion((mChar1Pos + mChar2Pos) * 0.5f,
                       { 1.0f, 0.9f, 0.3f, 1.0f }, 30);
        PlayBeep(1200, 60); PlayBeep(900, 60); PlayBeep(600, 60);
    } });

    mEvents.push_back({ 12.0f, false, [this]() {
        mAnimator02.PlayAnimation(Char2Clip::Jump, false);
        SpawnExplosion((mChar1Pos + mChar2Pos) * 0.5f + Vector3{0.3f,0.5f,0.f},
                       { 0.4f, 0.6f, 1.0f, 1.0f }, 25);
        PlayBeep(1500, 60); PlayBeep(1100, 60);
    } });

    mEvents.push_back({ 14.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Running, true);
        mAnimator02.PlayAnimation(Char2Clip::Running, true);
        PlayBeep(440, 80); PlayBeep(550, 80); PlayBeep(660, 80); PlayBeep(880, 150);
    } });

    mEvents.push_back({ 20.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Jump, false);
        SpawnExplosion(mChar1Pos + Vector3{0.f,1.f,0.f},
                       { 1.0f, 0.5f, 0.1f, 1.0f }, 40);
        PlayBeep(2000, 50); PlayBeep(1400, 50); PlayBeep(900, 50);
    } });

    mEvents.push_back({ 22.0f, false, [this]() {
        mAnimator02.PlayAnimation(Char2Clip::Jump, false);
        SpawnExplosion(mChar2Pos + Vector3{0.f,1.f,0.f},
                       { 0.3f, 0.5f, 1.0f, 1.0f }, 35);
        PlayBeep(2200, 50); PlayBeep(1600, 50);
    } });

    mEvents.push_back({ 26.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Running, true);
        mAnimator02.PlayAnimation(Char2Clip::Running, true);
        Vector3 mid = (mChar1Pos + mChar2Pos) * 0.5f;
        SpawnExplosion(mid, { 1.0f, 1.0f, 0.8f, 1.0f }, 30);
        PlayBeep(2500, 40); PlayBeep(1800, 40);
    } });

    mEvents.push_back({ 29.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Idle, true);
        SpawnExplosion(mChar1Pos + Vector3{0.f, 1.0f, 0.f},
                       { 1.0f, 0.5f, 0.0f, 1.0f }, 50);
        PlayBeep(800, 100); PlayBeep(600, 100); PlayBeep(400, 100);
    } });

    mEvents.push_back({ 33.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Idle, true);
        PlayBeep(500, 120); PlayBeep(600, 120); PlayBeep(500, 120);
    } });

    mEvents.push_back({ 35.0f, false, [this]() {
        mAnimator01.PlayAnimation(Char1Clip::Dying, false); 
        mAnimator02.PlayAnimation(Char2Clip::Running, true);
        PlayBeep(300, 150); PlayBeep(220, 200);
    } });

    mEvents.push_back({ 37.5f, false, [this]() {
        mRedExploded = true;
        SpawnExplosion(mChar1Pos + Vector3{0.f, 1.f, 0.f},
                       { 1.0f, 0.4f, 0.0f, 1.0f }, 120);
        SpawnExplosion(mChar1Pos + Vector3{0.f, 1.f, 0.f},
                       { 1.0f, 1.0f, 0.3f, 1.0f }, 80);
        PlayBeep(200, 300); PlayBeep(150, 250);
    } });

    mEvents.push_back({ 38.0f, false, [this]() {
        SpawnExplosion(mChar1Pos + Vector3{0.2f, 0.8f, 0.1f},
                       { 0.9f, 0.3f, 0.0f, 1.0f }, 60);
        PlayBeep(120, 200);
    } });

    mEvents.push_back({ 40.0f, false, [this]() {
        mAnimator02.PlayAnimation(Char2Clip::SillyDancing, true);
        SpawnExplosion(mChar2Pos + Vector3{0.f, 1.5f, 0.f},
                       { 0.3f, 0.8f, 1.0f, 1.0f }, 40);
        PlayBeep(660, 100); PlayBeep(880, 100); PlayBeep(1100, 100);
        PlayBeep(1320, 150); PlayBeep(1760, 300);
    } });

    mEvents.push_back({ 48.0f, false, []() {
        PlayBeep(880, 100); PlayBeep(990, 100); PlayBeep(1100, 100);
        PlayBeep(1320, 300);
    } });
}

void GameState::Terminate()
{
    mCharacter02.Terminate();
    mCharacter01.Terminate();
    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

    if (!mSceneRunning || mScenePaused) return;

    mSceneTime += deltaTime;

    mAnimator01.Update(deltaTime * mAnimSpeed);
    mAnimator02.Update(deltaTime * mAnimSpeed);

    UpdateCharacterMovement(deltaTime);

    mCharacter01.transform.position = mChar1Pos;
    mCharacter01.transform.rotation =
        Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, mChar1Yaw);

    mCharacter02.transform.position = mChar2Pos;
    mCharacter02.transform.rotation =
        Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, mChar2Yaw);

    if (!mRedExploded)
    {
        SpawnThrusterParticles(
            mChar1Pos + Vector3{ 0.f, 0.05f, 0.f },
            Vector3{ RandFloat(-0.3f,0.3f), 0.f, RandFloat(-0.3f,0.3f) },
            { 1.0f, 0.4f, 0.1f, 1.0f }, 2);
    }
    SpawnThrusterParticles(
        mChar2Pos + Vector3{ 0.f, 0.05f, 0.f },
        Vector3{ RandFloat(-0.3f,0.3f), 0.f, RandFloat(-0.3f,0.3f) },
        { 0.2f, 0.5f, 1.0f, 1.0f }, 2);

    if (mSceneTime > 14.0f && mSceneTime < 30.0f)
    {
        if (rand() % 5 == 0)
        {
            Vector3 mid = (mChar1Pos + mChar2Pos) * 0.5f + Vector3{ 0.f,1.f,0.f };
            SpawnExplosion(mid, { 1.0f, 1.0f, 0.6f, 1.0f }, 6);
        }
    }

    FireEventsUpTo(mSceneTime);

    UpdateParticles(deltaTime);

    if (mSceneTime >= kSceneDuration)
        mSceneRunning = false;
}

void GameState::UpdateCharacterMovement(float deltaTime)
{
    const float t = mSceneTime;

    if (t < 8.0f)
    {
        float alpha = t / 8.0f;
        mChar1Pos = Math::Lerp(Vector3{ -3.5f, 0.f, 0.f },
            Vector3{ -1.2f,  0.f, 0.f }, alpha);
        mChar2Pos = Math::Lerp(Vector3{ 3.5f, 0.f, 0.f },
            Vector3{ 1.2f,  0.f, 0.f }, alpha);
        mChar1Yaw = 0.0f;
        mChar2Yaw = Math::Constants::Pi;
    }
    else if (t < 14.0f)
    {
        mChar1Pos = { -1.2f, 0.f, 0.f };
        mChar2Pos = { 1.2f, 0.f, 0.f };
        mChar1Yaw = 0.0f;
        mChar2Yaw = Math::Constants::Pi;
    }
    else if (t < 35.0f)
    {
        float orbit = (t - 14.0f) * 0.25f;  
        float radius = 1.8f;
        mChar1Pos = { cosf(orbit) * radius,       0.f, sinf(orbit) * radius };
        mChar2Pos = { cosf(orbit + Math::Constants::Pi) * radius,
                      0.f,
                      sinf(orbit + Math::Constants::Pi) * radius };

        mChar1Yaw = orbit + Math::Constants::Pi * 0.5f;
        mChar2Yaw = orbit + Math::Constants::Pi * 1.5f;
    }
    else if (t < 40.0f)
    {
        float orbit = (35.0f - 14.0f) * 0.25f;
        float radius = 1.8f;
        mChar1Pos = { cosf(orbit) * radius, 0.f, sinf(orbit) * radius };

        float b = (t - 14.0f) * 0.25f;
        mChar2Pos = { cosf(b + Math::Constants::Pi) * radius,
                      0.f,
                      sinf(b + Math::Constants::Pi) * radius };
        mChar2Yaw = b + Math::Constants::Pi * 1.5f;
    }
    else
    {
        float alpha = Math::Min((t - 40.0f) / 5.0f, 1.0f);
        mChar2Pos = Math::Lerp(mChar2Pos, Vector3{ 0.f, 0.f, 0.f }, alpha * deltaTime * 2.0f);
        mChar2Yaw += deltaTime * 0.5f;   
    }
}

void GameState::Render()
{
    SimpleDraw::AddGroundPlane(20.0f, Colors::DimGray);
    SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();

    if (!mRedExploded)
        mStandardEffect.Render(mCharacter01);

    mStandardEffect.Render(mCharacter02);

    mStandardEffect.End();

    if (mDrawSkeleton01 && !mRedExploded)
    {
        AnimationUtil::BoneTransforms bt;
        AnimationUtil::ComputeBoneTransforms(mCharacter01.modelId, bt, &mAnimator01);
        AnimationUtil::DrawSkeleton(mCharacter01.modelId, bt);
        SimpleDraw::Render(mCamera);
    }
    if (mDrawSkeleton02)
    {
        AnimationUtil::BoneTransforms bt;
        AnimationUtil::ComputeBoneTransforms(mCharacter02.modelId, bt, &mAnimator02);
        AnimationUtil::DrawSkeleton(mCharacter02.modelId, bt);
        SimpleDraw::Render(mCamera);
    }

    RenderParticles();
    SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
    ImGui::Begin("Space Battle Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    float remaining = Math::Max(kSceneDuration - mSceneTime, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_Text,
        remaining > 10.0f ? ImVec4(0.2f, 1.0f, 0.4f, 1.0f)
        : ImVec4(1.0f, 0.3f, 0.1f, 1.0f));
    ImGui::Text("Timer   : %.1f / %.0f s", mSceneTime, kSceneDuration);
    ImGui::Text("Remaining: %.1f s", remaining);
    ImGui::PopStyleColor();
    ImGui::ProgressBar(Math::Min(mSceneTime / kSceneDuration, 1.0f),
        ImVec2(-1.0f, 0.0f));

    ImGui::Separator();

    const char* phase = "INTRO ！ Face-Off";
    if (mSceneTime > 40.0f) phase = "PHASE 5 ！ Blue Victory Dance!";
    else if (mSceneTime > 35.0f) phase = "PHASE 4 ！ Red is Dying!";
    else if (mSceneTime > 14.0f) phase = "PHASE 3 ！ DOGFIGHT!";
    else if (mSceneTime > 8.0f) phase = "PHASE 2 ！ Stare-Down";
    ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "Phase: %s", phase);

    ImGui::Separator();

    const char* clip1Names[] = { "Idle", "Running", "Jump", "Dying" };
    const char* clip2Names[] = { "Idle", "Running", "SillyDancing", "Jump" };

    ImGui::Text("Char01 (Red)  clips loaded: Idle / Running / Jump / Dying");
    ImGui::Text("Char02 (Blue) clips loaded: Idle / Running / SillyDancing / Jump");

    ImGui::Separator();

    ImGui::DragFloat("Anim Speed", &mAnimSpeed, 0.05f, 0.0f, 3.0f);

    ImGui::Text("Active Particles: %d / %d", (int)mParticles.size(), kMaxParticles);

    ImGui::Checkbox("Show Skeleton Char01", &mDrawSkeleton01);
    ImGui::SameLine();
    ImGui::Checkbox("Show Skeleton Char02", &mDrawSkeleton02);

    ImGui::Separator();

    if (mSceneRunning)
    {
        if (mScenePaused)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.1f, 1.0f));
            if (ImGui::Button("Resume")) mScenePaused = false;
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.6f, 0.0f, 1.0f));
            if (ImGui::Button("Pause"))  mScenePaused = true;
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        if (ImGui::Button("Restart")) ResetScene();
    }
    else
    {
        ImGui::TextColored({ 1.0f, 0.6f, 0.0f, 1.0f }, "-- SCENE COMPLETE --");
        if (ImGui::Button("Replay")) ResetScene();
    }

    ImGui::Separator();
    ImGui::TextDisabled("WASD + RMB = manual camera");
    ImGui::TextDisabled("Auto-orbit when no input");

    ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
    static float autoAngle = 0.0f;
    autoAngle += deltaTime * 0.10f;

    Vector3 midpoint = (mChar1Pos + mChar2Pos) * 0.5f;
    float camDist = 8.0f;
    float camH = 3.0f + sinf(mSceneTime * 0.2f) * 0.5f;

    Vector3 autoCamPos = {
        midpoint.x + cosf(autoAngle) * camDist,
        camH,
        midpoint.z + sinf(autoAngle) * camDist
    };

    InputSystem* input = InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
    bool manual = false;

    if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime);  manual = true; }
    if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime);  manual = true; }
    if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); manual = true; }
    if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); manual = true; }
    if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime);  manual = true; }
    if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime);  manual = true; }
    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * 0.4f * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * 0.4f * deltaTime);
        manual = true;
    }

    if (!manual)
    {
        mCamera.SetPosition(autoCamPos);
        mCamera.SetLookAt(midpoint + Vector3{ 0.f, 1.0f, 0.f });
    }
}

void GameState::SpawnThrusterParticles(const Vector3& pos, const Vector3& dir,
    const Color& col, int count)
{
    if ((int)mParticles.size() >= kMaxParticles) return;
    count = Math::Min(count, kMaxParticles - (int)mParticles.size());
    for (int i = 0; i < count; ++i)
    {
        Particle p;
        p.position = pos + Vector3{
            RandFloat(-0.1f, 0.1f), RandFloat(0.0f, 0.05f), RandFloat(-0.1f, 0.1f) };
        p.velocity = dir * RandFloat(0.5f, 2.0f) + Vector3{
            RandFloat(-0.2f,0.2f), RandFloat(0.1f,0.5f), RandFloat(-0.2f,0.2f) };
        p.color = col;
        p.lifetime = RandFloat(0.2f, 0.6f);
        p.maxLife = p.lifetime;
        p.size = RandFloat(0.02f, 0.06f);
        mParticles.push_back(p);
    }
}

void GameState::SpawnExplosion(const Vector3& pos, const Color& col, int count)
{
    if ((int)mParticles.size() >= kMaxParticles) return;
    count = Math::Min(count, kMaxParticles - (int)mParticles.size());
    for (int i = 0; i < count; ++i)
    {
        Particle p;
        p.position = pos + Vector3{
            RandFloat(-0.2f,0.2f), RandFloat(0.f,0.3f), RandFloat(-0.2f,0.2f) };
        float spd = RandFloat(1.5f, 6.0f);
        Vector3 dir = Math::Normalize(Vector3{
            RandFloat(-1.f,1.f), RandFloat(-0.5f,1.f), RandFloat(-1.f,1.f) });
        p.velocity = dir * spd;
        p.color = col;
        p.lifetime = RandFloat(0.5f, 1.5f);
        p.maxLife = p.lifetime;
        p.size = RandFloat(0.05f, 0.18f);
        mParticles.push_back(p);
    }
}

void GameState::UpdateParticles(float deltaTime)
{
    for (auto& p : mParticles)
    {
        p.position += p.velocity * deltaTime;
        p.velocity = p.velocity * 0.96f;
        p.velocity.y -= 2.5f * deltaTime;  
        p.lifetime -= deltaTime;
        p.color.a = Math::Max(p.lifetime / p.maxLife, 0.0f);
    }
    mParticles.erase(
        std::remove_if(mParticles.begin(), mParticles.end(),
            [](const Particle& p) { return p.lifetime <= 0.0f; }),
        mParticles.end());
}

void GameState::RenderParticles()
{
    for (const auto& p : mParticles)
        SimpleDraw::AddSphere(4, 4, p.size, p.position, p.color);
}

void GameState::FireEventsUpTo(float time)
{
    for (auto& ev : mEvents)
    {
        if (!ev.fired && time >= ev.triggerTime)
        {
            ev.fired = true;
            if (ev.action) ev.action();
        }
    }
}

void GameState::ResetScene()
{
    mSceneTime = 0.0f;
    mRedExploded = false;
    mSceneRunning = true;
    mScenePaused = false;
    mParticles.clear();

    mChar1Pos = { -3.5f, 0.f, 0.f };
    mChar2Pos = { 3.5f, 0.f, 0.f };
    mChar1Yaw = 0.0f;
    mChar2Yaw = Math::Constants::Pi;

    mCharacter01.transform.position = mChar1Pos;
    mCharacter02.transform.position = mChar2Pos;

    mAnimator01.PlayAnimation(Char1Clip::Idle, true);
    mAnimator02.PlayAnimation(Char2Clip::Idle, true);

    for (auto& ev : mEvents) ev.fired = false;
}