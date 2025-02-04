//
//  WordMatchingCard.cpp
//  PehlaSchool
//
//  Created by timewalker on 6/30/16.
//
//


#include "LetterMatchingCard.hpp"
#include "Utils/TodoUtil.h"
#include <string>

#define GROUP_MAX_COUNT 10
#define SCALE_FACTOR 0.7f


namespace {
}  // unnamed namespace


bool LetterMatchingCard::init()
{
    if (!Widget::init()) { return false; }

    isMatchDone = false;
    return true;
}

void LetterMatchingCard::setImage(int level, int type, int number,
                                  const std::string& cardImageName)
{
    auto spriteName = std::string("NumberMatching/Images/Letter/") + cardImageName;
    Node* background = Sprite::create(spriteName);
    if (!background) {
        background = TodoUtil::createLabel(cardImageName, 50, Size::ZERO, "fonts/mukta-bold.ttf", Color4B::BLACK);
    }

    auto contentSize = background->getContentSize();

    background->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    background->setPosition(contentSize / 2);
    addChild(background);

    id = number;
    setContentSize(contentSize);
    setCascadeOpacityEnabled(true);
    
    createShiningParticle();

}

bool LetterMatchingCard::isTouchedIn(Point pt)
{
    Point pos = getPosition();
    
    Rect bBox = this->getBoundingBox();
    bBox.origin.x = pos.x -(bBox.size.width / 2.0f);
    bBox.origin.y = pos.y -(bBox.size.height / 2.0f);
    
    //    CCLOG("scale : %f, pos : (%f,%f) bBox : (%f,%f)",getScale(),pos.x,pos.y,bBox.origin.x,bBox.origin.y);
    
    return bBox.containsPoint(pt);
}

bool shouldBecomePair(LetterMatchingCard* cardA, LetterMatchingCard* cardB) {
    if (!cardA || !cardB) { return false; }
    if (cardA->isMatchDone || cardB->isMatchDone) { return false; }

    auto convertToWorldSpace = [](Node* node, Rect rect) {
        auto origin = node->convertToWorldSpace(rect.origin);
        auto diagonal = node->convertToWorldSpace(rect.origin + rect.size);
        
        return Rect(origin, Size(diagonal - origin));
    };
    
    Rect rectA = convertToWorldSpace(cardA, Rect(Point::ZERO, cardA->getContentSize()));
    Rect rectB = convertToWorldSpace(cardB, Rect(Point::ZERO, cardB->getContentSize()));
    
    return rectA.intersectsRect(rectB);
}

bool shouldKeepAsPair(LetterMatchingCard* cardA, LetterMatchingCard* cardB) {
    return shouldBecomePair(cardA, cardB);
}

Size LetterMatchingCard::defaultSize()
{
    return Size(450, 570);
}


void LetterMatchingCard::createShiningParticle()
{
    _shiningParticleNode = Node::create();
    
    ParticleSystemQuad* particleEffect = nullptr;

    auto createParticleEffect = [&](std::string name, std::string plist) {
        particleEffect = ParticleSystemQuad::create(plist);
        particleEffect->setName(name);
        particleEffect->setScale(1.8f);
        particleEffect->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        particleEffect->setPosition(Vec2::ZERO);
        particleEffect->stopSystem();
        _shiningParticleNode->addChild(particleEffect);
    };
    
    createParticleEffect("particle1", "Common/Effects/Particle/shining_particle_blur.plist");
    createParticleEffect("particle2", "Common/Effects/Particle/shining_particle_circle.plist");
    createParticleEffect("particle3", "Common/Effects/Particle/shining_particle_star.plist");
    
    _shiningParticleNode->setPosition(this->getContentSize()/2);
    addChild(_shiningParticleNode);
    
}

void LetterMatchingCard::startParticle()
{
    _shiningParticleNode->getChildByName<ParticleSystemQuad*>("particle1")->resetSystem();
    _shiningParticleNode->getChildByName<ParticleSystemQuad*>("particle2")->resetSystem();
    _shiningParticleNode->getChildByName<ParticleSystemQuad*>("particle3")->resetSystem();
}

void LetterMatchingCard::stopParticle()
{
    _shiningParticleNode->getChildByName<ParticleSystemQuad*>("particle1")->stopSystem();
    _shiningParticleNode->getChildByName<ParticleSystemQuad*>("particle2")->stopSystem();
    _shiningParticleNode->getChildByName<ParticleSystemQuad*>("particle3")->stopSystem();
}

void LetterMatchingCard::setLink(LetterMatchingCard *card)
{
    if (card) {
        isLinked = true;
        linkedTarget = card;
        startParticle();
    } else {
        isLinked = false;
        linkedTarget = nullptr;
        stopParticle();
        
        if (!isTouched) runAction(EaseElasticOut::create(ScaleTo::create(0.3f, defaultScale)));
    }
    
}

