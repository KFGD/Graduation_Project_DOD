#pragma once

class IWorldController;

class IMode
{
protected:
	explicit IMode() = default;
	virtual ~IMode() = default;

public:
	virtual void	Active(IWorldController* worldController) = 0;
	virtual void	InActive(IWorldController* worldController) = 0;
	virtual void	Update(IWorldController* worldController) = 0;
};