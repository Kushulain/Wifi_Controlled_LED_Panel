
class CoolDown
{
  unsigned long cooldownTime;
  long lastGoTime;

  public :
    CoolDown(unsigned long duration)
    {
       SetCooldown(duration);
    }

    void Go()
    {
      Go(cooldownTime);
    }
    
    void Go(unsigned long duration)
    {
      lastGoTime = millis();
    }
    
    void SetCooldown(unsigned long duration)
    {
      cooldownTime = duration;
      lastGoTime = -duration;
    }
    
    bool TryGo(unsigned long duration)
    {
      if (isRunning())
        return false;
      else
        Go(duration);
      return true;
    }
    
    bool TryGo()
    {
      return TryGo(cooldownTime);
    }

    bool isRunning()
    {
      //Serial.println("millis():" + String(millis()) + " lastGoTime:" + lastGoTime + " cooldownTime:" + cooldownTime);
      return millis() < (lastGoTime + cooldownTime); 
    }
    
    operator bool() 
    {
      return isRunning();
    }
};

