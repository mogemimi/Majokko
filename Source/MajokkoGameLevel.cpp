//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MajokkoGameLevel.hpp"
#include "RenderLayers/GameWorldLayer.hpp"
#include "Breakable.hpp"
#include "Actor.hpp"

namespace Majokko {
namespace {

struct GameCommandShot {
};

struct GameCommandMove {
    Vector2 Direction = Vector2::Zero;
};

struct GameCommandStop {
};

class PlayerCommandTranslator {
public:
    void Translate(Duration const& frameDuration, KeyboardState const& keyboardState, EventQueue & eventQueue);

private:
    //bool spaceKeyPressed = false;
};

void PlayerCommandTranslator::Translate(Duration const& frameDuration, KeyboardState const& keyboardState, EventQueue & eventQueue)
{
    {
        Vector2 direction = Vector2::Zero;

        if (keyboardState.IsKeyDown(Keys::W)) {
            direction.Y += 1.0f;
        }
        if (keyboardState.IsKeyDown(Keys::A)) {
            direction.X += -1.0f;
        }
        if (keyboardState.IsKeyDown(Keys::S)) {
            direction.Y += -1.0f;
        }
        if (keyboardState.IsKeyDown(Keys::D)) {
            direction.X += 1.0f;
        }

        if (direction.LengthSquared() > std::numeric_limits<float>::epsilon()) {
            eventQueue.Enqueue<GameCommandMove>(direction);
        }
        else {
            eventQueue.Enqueue<GameCommandStop>();
        }
    }
    {
        if (keyboardState.IsKeyDown(Keys::Space))
        {
            eventQueue.Enqueue<GameCommandShot>();
        }
    }
//    {
//        if (!spaceKeyPressed && keyboardState.IsKeyDown(Keys::Space))
//        {
//            eventQueue.Enqueue<GameCommandShot>();
//            spaceKeyPressed = true;
//        }
//
//        if (spaceKeyPressed && keyboardState.IsKeyUp(Keys::Space))
//        {
//            spaceKeyPressed = false;
//        }
//    }
}

struct ActionHelper {
    static std::unique_ptr<MoveToAction> MoveTo(AnimationTimeInterval const& duration, Vector2 const& position)
    {
        return std::make_unique<MoveToAction>(duration, position);
    }

    static std::unique_ptr<ScaleToAction> ScaleTo(AnimationTimeInterval const& duration, Vector2 const& scale)
    {
        return std::make_unique<ScaleToAction>(duration, scale);
    }

    static std::unique_ptr<ScaleToAction> ScaleTo(AnimationTimeInterval const& duration, float scale)
    {
        return std::make_unique<ScaleToAction>(duration, Vector2{scale, scale});
    }

    static std::unique_ptr<TintToAction> TintTo(AnimationTimeInterval const& duration, Color const& color)
    {
        return std::make_unique<TintToAction>(duration, color);
    }

    static std::unique_ptr<RemoveActorAction> RemoveActor()
    {
        return std::make_unique<RemoveActorAction>();
    }

    template <typename...Args>
    static std::unique_ptr<SequenceAction> Sequence(std::unique_ptr<Action> && action, Args &&... actions)
    {
        return std::make_unique<SequenceAction>(std::move(action), std::move(actions)...);
    }
};

}// unnamed namespace
//-----------------------------------------------------------------------
MajokkoGameLevel::MajokkoGameLevel(GameHost & gameHost, Timer & gameTimerIn, GameWorld & gameWorld, RenderLayerCompositor & compositor)
    : gameTimer(gameTimerIn)
    , spawnTimer(*gameHost.Clock())
    , castingTimer(*gameHost.Clock())
{
    auto graphicsDevice = gameHost.GraphicsDevice();
    auto assets = gameHost.AssetManager();
    {
        mainCamera = factory.CreateCamera(gameWorld);
        littleWitch = factory.CreateLittleWitch(gameWorld, *graphicsDevice, *assets);
    }
    {
        auto background = gameWorld.CreateObject();
        background.AddComponent<Transform2D>();
        auto & rectangle = background.AddComponent<RectangleRenderable>();
        Color color1 = {84-40, 133-40, 153-40, 255};
        //Color color2 = {84-40, 133-40, 153-40, 255};
        Color color2 = {190, 204, 207, 255};
        rectangle.LeftTopColor(color1);
        rectangle.RightTopColor(color1);
        rectangle.LeftBottomColor(color2);
        rectangle.RightBottomColor(color2);
        auto bounds = gameHost.Window()->GetClientBounds();
        rectangle.BoundingBox({0, 0, bounds.Width, bounds.Height});
        rectangle.DrawOrder = -1000.0f;
    }
//    {
//        auto entity = gameWorld.CreateObject();
//        entity.AddComponent<Transform2D>();
//        auto particleClip = std::make_shared<ParticleClip>(Detail::ParticleLoader::Load(*assets, "Particles/explosion_smoke.json"));
//        auto texture = assets->Load<Texture2D>("Particles/explosion_smoke.png");
//        auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
//        auto & particleSystem = entity.AddComponent<ParticleSystem>(particleClip);
//        particleSystem.Loop(true);
//
//        entity.AddComponent<ParticleRenderable>(texture, blendState);
//    }
    {
        auto layer = std::make_shared<GameWorldLayer>(gameHost, gameWorld);
        layer->Camera(mainCamera);
        compositor.AddLayer(layer);
    }
//    {
//        constexpr float timeScale = 0.3f;
//        gameTimer.Scale(timeScale);
//        spawnTimer.Scale(timeScale);
//        castingTimer.Scale(timeScale);
//    }
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::Update(GameHost & gameHost, GameWorld & gameWorld)
{
    auto UpdateMovable = [&]
    {
        for (auto & entity: gameWorld.QueryComponents<Movable, Transform2D>())
        {
            auto transform = entity.Component<Transform2D>();
            auto movable = entity.Component<Movable>();
            transform->Position += (movable->Velocity * gameTimer.GetFrameDuration().count());
        }
    };

    auto UpdateBullet = [&]
    {
        for (auto & entity: gameWorld.QueryComponents<Bullet, Transform2D>())
        {
            constexpr float speed = 900.0f;
            Vector2 direction{1.0f, 0.0f};
            auto transform = entity.Component<Transform2D>();
            transform->Position += (direction * speed * gameTimer.GetFrameDuration().count());

            if (transform->Position.X > 1000.0f) {
                entity.Destroy();
            }
        }
    };

    auto DestroyUnusedBreakable = [&]
    {
        for (auto & entity: gameWorld.QueryComponents<Breakable, Transform2D>())
        {
            auto transform = entity.Component<Transform2D>();
            if (transform->Position.X < -1000.0f) {
                entity.Destroy();
            }
        }
    };

    auto SpawnGhost = [&]
    {
        constexpr Duration spawnInterval {0.7};
        if (spawnTimer.GetTotalTime() >= spawnInterval)
        {
            auto graphicsDevice = gameHost.GraphicsDevice();
            auto assets = gameHost.AssetManager();

            for (int i = 0; i < 3; ++i) {
                Vector2 position = {600.0f, -100.0f + 100.0f * i};
                factory.CreateGhost(gameWorld, *graphicsDevice, *assets, position);
            }
            spawnTimer.Reset();
        }
    };

    auto CheckCollision = [&]
    {
        auto breakables = gameWorld.QueryComponents<Breakable, Collider2D>();

        for (auto & bullet: gameWorld.QueryComponents<Bullet, Collider2D>())
        {
            if (!bullet) {
                continue;
            }

            for (auto & enemy: breakables)
            {
                if (!enemy) {
                    continue;
                }

                POMDOG_ASSERT(bullet);

                auto entityCollider = enemy.Component<Collider2D>();
                auto entityTransform = enemy.Component<Transform2D>();

                auto bulletCollider = bullet.Component<Collider2D>();
                auto bulletTransform = bullet.Component<Transform2D>();

                auto entityBounds = entityCollider->Bounds;
                auto bulletBounds = bulletCollider->Bounds;

                entityBounds.Center += entityTransform->Position;
                entityBounds.Radius *= std::max(entityTransform->Scale.X, entityTransform->Scale.Y);

                bulletBounds.Center += bulletTransform->Position;
                bulletBounds.Radius *= std::max(bulletTransform->Scale.X, bulletTransform->Scale.Y);

                if (ContainmentType::Contains == entityBounds.Contains(bulletBounds))
                {
                    auto breakable = enemy.Component<Breakable>();

                    if (breakable->IsDead()) {
                        break;
                    }

                    breakable->Damage(10.0f);

                    auto actor = enemy.Component<Actor>();
                    actor->RunAction(ActionHelper::Sequence(
                        ActionHelper::TintTo(std::chrono::milliseconds(120), Color{170,170,170,255}),
                        ActionHelper::TintTo(std::chrono::milliseconds(110), Color{220,220,220,255}),
                        ActionHelper::TintTo(std::chrono::milliseconds(120), Color{170,170,170,255}),
                        ActionHelper::TintTo(std::chrono::milliseconds(110), Color::White)));

                    bullet.Destroy();
                    break;
                }
            }
        }
    };

    auto ApplyDamageToBreakable = [&]
    {
        for (auto & entity: gameWorld.QueryComponents<Breakable>())
        {
            auto breakable = entity.Component<Breakable>();
            breakable->ApplyDamage();

            if (breakable->IsDead())
            {
                auto enemy = entity.Component<Enemy>();

                if (enemy->State == EnemyState::Dead) {
                    continue;
                }

                auto actor = entity.Component<Actor>();

                auto transform = entity.Component<Transform2D>();
                auto originalScale = transform->Scale;
                actor->RunAction(ActionHelper::Sequence(
                    ActionHelper::ScaleTo(std::chrono::milliseconds(90), originalScale * Vector2{1.1f, 1.1f}),
                    ActionHelper::ScaleTo(std::chrono::milliseconds(70), originalScale * Vector2{0.96f, 0.96f}),
                    ActionHelper::ScaleTo(std::chrono::milliseconds(110), originalScale * Vector2{1.2f, 1.2f})));

                actor->RunAction(ActionHelper::Sequence(
                    ActionHelper::TintTo(std::chrono::milliseconds(320), Color{170,170,170,0}),
                    ActionHelper::RemoveActor()));

                enemy->State = EnemyState::Dead;
            }
        }
    };

    auto DestroyUnusedParticles = [&]
    {
        for (auto & entity: gameWorld.QueryComponents<ParticleSystem>())
        {
            auto paritcleSystem = entity.Component<ParticleSystem>();
            if (!paritcleSystem->IsAlive()) {
                entity.Destroy();
            }
        }
    };

    UpdatePlayerInput(gameHost, gameWorld);
    UpdateMovable();
    UpdateBullet();
    DestroyUnusedBreakable();
    CheckCollision();
    ApplyDamageToBreakable();
    SpawnGhost();
    DestroyUnusedParticles();
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::UpdatePlayerInput(GameHost & gameHost, GameWorld & gameWorld)
{
    EventQueue eventQueue;
    eventQueue.Connect([&](Event const& args)
    {
        if (auto event = args.As<GameCommandMove>()) {
            constexpr float MaxSpeed = 300.0f;
            constexpr float MaxThrust = 1000.0f;

            auto normalizedDirection = Vector2::Normalize(event->Direction);
            auto movable = littleWitch.Component<Movable>();

            constexpr float mass = 1.0f;
            movable->Thrust = (MaxThrust * normalizedDirection);
            movable->Velocity += (mass * movable->Thrust * gameTimer.GetFrameDuration().count());

            if (movable->Velocity.LengthSquared() > MaxSpeed * MaxSpeed) {
                movable->Thrust = Vector2::Zero;
                movable->Velocity = Vector2::Normalize(movable->Velocity) * MaxSpeed;
            }
        }
        else if (args.Is<GameCommandStop>()) {
            auto movable = littleWitch.Component<Movable>();

            movable->Thrust = Vector2::Zero;
            movable->Velocity = Vector2::Zero;
        }
        else if (args.Is<GameCommandShot>()) {
            constexpr Duration castingTime = std::chrono::milliseconds(95);

            if (castingTimer.GetTotalTime() >= castingTime)
            {
                auto assets = gameHost.AssetManager();
                auto position = littleWitch.Component<Transform2D>()->Position + Vector2{50.0f, 30.0f};
                factory.CreateBullet(gameWorld, *assets, position + Vector2{0.0f, 8.0f});
                factory.CreateBullet(gameWorld, *assets, position + Vector2{0.0f, -8.0f});

                castingTimer.Reset();
            }
        }
    });

    {
        auto movable = littleWitch.Component<Movable>();
        auto anim = littleWitch.Component<Animator>();

        if (movable->Velocity.LengthSquared() == 0.0f) {
            if (anim->GetCurrentStateName() != "Idle") {
                anim->CrossFade("Idle", std::chrono::milliseconds(400));
            }
        }
        else {
            if (anim->GetCurrentStateName() != "Run") {
                anim->CrossFade("Run", std::chrono::milliseconds(300));
            }

            constexpr float MaxSpeed = 300.0f;
            const auto min = -MaxSpeed;
            const auto max = MaxSpeed;
            const auto range = max - min;
            const auto normalizedWeight = (movable->Velocity.X - min) / range;

            anim->SetFloat("Weight", normalizedWeight);
        }
    }

    auto keyboard = gameHost.Keyboard();
    auto keyboardState = keyboard->GetState();

    static PlayerCommandTranslator translator;
    translator.Translate(gameTimer.GetFrameDuration(), keyboardState, eventQueue);

    eventQueue.Emit();
}
//-----------------------------------------------------------------------
} // namespace Majokko
