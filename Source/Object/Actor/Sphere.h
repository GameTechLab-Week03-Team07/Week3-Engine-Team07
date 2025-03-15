﻿#pragma once
#include "Object/Actor/Actor.h"

class ASphere : public AActor
{
	DECLARE_CLASS(ASphere, AActor)

public:
	ASphere();
	virtual ~ASphere() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

