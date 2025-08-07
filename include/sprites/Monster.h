#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Monster
{
public:
    enum class State { Idle, Walk, Attack1, Attack2, Hurt, Death };
    
private:
    sf::Sprite m_sprite;
    
    const sf::Texture& m_idleTexture;
    const sf::Texture& m_walkTexture;
    const sf::Texture& m_attack1Texture;
    const sf::Texture& m_attack2Texture;
    const sf::Texture& m_hurtTexture;
    const sf::Texture& m_deathTexture;

    
    std::vector<sf::IntRect> m_idleFrames;
    std::vector<sf::IntRect> m_walkFrames;
    std::vector<sf::IntRect> m_attack1Frames;
    std::vector<sf::IntRect> m_attack2Frames;
    std::vector<sf::IntRect> m_hurtFrames;
    std::vector<sf::IntRect> m_deathFrames;

    float m_frameTime;
    float m_elapsedTime;
    int m_currentFrame;

    State m_state;
    bool m_facingRight;

    
    float m_leftBound;
    float m_rightBound;
    float m_speed;
    float m_direction;

public:
    Monster(const sf::Texture& idleTex,
            const sf::Texture& walkTex,
            const sf::Texture& attack1Tex,
            const sf::Texture& attack2Tex,
            const sf::Texture& hurtTex,
            const sf::Texture& deathTex,
            float leftBound, float rightBound);

    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    // State control
    void SetState(State newState);
    void Attack1();
    void Attack2();
    void Hurt();
    void Die();
    void Flip();

    sf::Sprite& GetSprite();
    State getState(){return m_state;};
    bool GetIsFaceingRight(){return m_facingRight;};
};
