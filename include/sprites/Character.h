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

        int m_rockAmmo;
        int m_levelWidth;
        int m_currentFrame;

        float m_frameTime;
        float m_elapsedTime;
        float m_throwAnimTime;
        float m_throwAnimDuration; 
        float m_speed; 
        float m_gravity;   
        float m_jumpStrength; 
        float m_deathDelayTimer;
        float m_deathDelayDuration;
        bool m_deathEnded;

        
        bool m_isOnGround;
        bool m_isWalking;
        bool m_isJumping;
        bool m_isThrow;
        bool m_isThrowingAnimation;
        bool m_isDead;

        RockState m_rockState;

        const sf::Texture& m_walkTexture;
        const sf::Texture& m_idleTexture;
        const sf::Texture& m_jumpText;
        const sf::Texture& m_deathText;
        const sf::Texture& m_rockTex;
        const sf::Texture& m_throwTex;

        sf::Vector2f m_velocity;
        sf::Vector2f m_rockVelocity;  // Separate velocity for rock

        
        
        

        std::vector<sf::IntRect> m_idleFrames;
        std::vector<sf::IntRect> m_walkFrames;
        std::vector<sf::IntRect> m_jumpFrames;
        std::vector<sf::IntRect> m_throwFrames;
        std::vector<sf::IntRect> m_deathFrames;

        
        
        

    public:
        Character(const sf::Texture& idleText, const sf::Texture& walkText, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& deathText, const sf::Texture& rockText, int levelWidth);
        virtual ~Character() = default;

        virtual void Update(float dt, bool isWalking, const std::vector<sf::Sprite*>& platforms);
        virtual void Draw(sf::RenderWindow& window);

        void jump();
        void throwRock();
        void kill();

        void SetPosition(float x, float y);
        void setIsWalking(bool isWalking){m_isWalking = isWalking;};
        void setIsJumping(bool isJumping){m_isJumping = isJumping;};
        void setVelocity(sf::Vector2f velocity) {m_velocity = velocity;};

        sf::Sprite& GetSprite();
        sf::Sprite& getRock() {return m_rock;};
        bool getWalking() const {return m_isWalking;};
        bool getJumping() const {return m_isJumping;};
        bool getIsOnGround() const {return m_isOnGround;};
        bool getDidDeathEnd() const {return m_deathEnded;};
        float getSpeed() const {return m_speed;};
        RockState getRockState() const {return m_rockState;};
        sf::Vector2f getVelocity() const {return m_velocity;};
        
        // Collision detection methods
        bool checkRockCollision(const sf::Sprite& otherSprite);
        void updateRockPhysics(float dt, const std::vector<sf::Sprite*>& platforms);
        void pickupRock();
        void resetRock();
        bool checkPlayerCollision(const std::vector<sf::Sprite*>& platforms, float directionX);
        bool checkPlayerCollisionDuringMovement(const std::vector<sf::Sprite*>& platforms, float directionX);
        bool checkVerticalCollision(const std::vector<sf::Sprite*>& platforms, sf::Vector2f& landingPosition);
        bool isOnPlatform(const std::vector<sf::Sprite*>& platforms);
        

};

