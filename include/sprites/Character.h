#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "../core/Config.h"


class Character 
{
    public:
        enum class RockState { None, Flying, Landed };
    protected:
        sf::Sprite m_sprite;
        sf::Sprite m_rock;

        RockState m_rockState;

        const sf::Texture& m_walkTexture;
        const sf::Texture& m_idleTexture;
        const sf::Texture& m_jumpText;
        const sf::Texture& m_rockTex;
        const sf::Texture& m_throwTex;

        sf::Vector2f m_velocity;
        sf::Vector2f m_rockVelocity;  // Separate velocity for rock

        int m_rockAmmo;
        int m_levelWidth;
        float m_speed; 
        float m_gravity;   
        float m_jumpStrength; 
        bool m_isOnGround;
        

        std::vector<sf::IntRect> m_idleFrames;
        std::vector<sf::IntRect> m_walkFrames;
        std::vector<sf::IntRect> m_jumpFrames;
        std::vector<sf::IntRect> m_throwFrames;

        float m_frameTime;
        float m_elapsedTime;
        int m_currentFrame;
        bool m_isWalking;
        bool m_isJumping;
        bool m_isThrow;
        bool m_isThrowingAnimation;
        float m_throwAnimTime;
        float m_throwAnimDuration; 

    public:
        Character(const sf::Texture& idleText, const sf::Texture& walkText, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& rockText, int levelWidth);
        virtual ~Character() = default;

        virtual void Update(float dt, bool isWalking, const std::vector<sf::Sprite*>& platforms);
        virtual void Draw(sf::RenderWindow& window);

        void jump();
        void throwRock();

        void SetPosition(float x, float y);
        void setIsWalking(bool isWalking){m_isWalking = isWalking;};
        void setIsJumping(bool isJumping){m_isJumping = isJumping;};
        void setVelocity(sf::Vector2f velocity) {m_velocity = velocity;};

        sf::Sprite& GetSprite();
        bool getWalking() const {return m_isWalking;};
        bool getJumping() const {return m_isJumping;};
        bool getIsOnGround() const {return m_isOnGround;};
        float getSpeed() const {return m_speed;};
        RockState getRockState() const {return m_rockState;};
        sf::Vector2f getVelocity() const {return m_velocity;};
        
        // Collision detection methods
        bool checkRockCollision(const sf::Sprite& otherSprite);
        void updateRockPhysics(float dt, const std::vector<sf::Sprite*>& platforms);
        void pickupRock();
        bool checkPlayerCollision(const std::vector<sf::Sprite*>& platforms, float directionX);
        bool checkPlayerCollisionDuringMovement(const std::vector<sf::Sprite*>& platforms, float directionX);
        bool checkVerticalCollision(const std::vector<sf::Sprite*>& platforms, sf::Vector2f& landingPosition);
        bool isOnPlatform(const std::vector<sf::Sprite*>& platforms);


};

