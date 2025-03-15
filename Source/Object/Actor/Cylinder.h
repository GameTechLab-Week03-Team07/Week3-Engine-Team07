﻿#pragma once
#include "Actor.h"

class ACylinder : public AActor
{
    DECLARE_CLASS(ACylinder, AActor)

public:
    ACylinder();
    virtual ~ACylinder() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;
};

