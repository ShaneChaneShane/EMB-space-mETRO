const { DrynessLevel, SunlightLevel, RainStatus } = require("./enums");

module.exports = async function evaluateSensorData(
  humidityEnv,
  humidityClothes,
  rain,
  temperature,
  light
) {
  // TO-DO: IMPLEMENT REAL LOGIC HERE

  // SOME IMAGINARY LOGIC TO DETERMINE  RAIN
  const rainStatus = rain > 500 ? RainStatus.RAINING : RainStatus.NOT_RAINING;

  // SOME IMAGINARY LOGIC TO DETERMINE SUNLIGHT LEVEL
  const sunlightLevel =
    light > 700
      ? SunlightLevel.HIGH
      : light > 300
      ? SunlightLevel.MODERATE
      : SunlightLevel.LOW;
  const humidityDiff = humidityClothes - humidityEnv;

  const step = (DrynessLevel.MAX - DrynessLevel.MIN) / 5;

  // SOME IMAGINARY LOGIC TO DETERMINE DRYNESS LEVEL
  const drynessLevel =
    humidityDiff < 10
      ? DrynessLevel.MIN
      : humidityDiff < 30
      ? DrynessLevel.MIN + step
      : humidityDiff < 50
      ? DrynessLevel.MIN + 2 * step
      : humidityDiff < 70
      ? DrynessLevel.MIN + 3 * step
      : humidityDiff < 90
      ? DrynessLevel.MIN + 4 * step
      : DrynessLevel.MAX;

  return {
    humidity: humidityEnv,
    rain: rainStatus,
    sun: sunlightLevel,
    temperature: temperature,
    dryness: drynessLevel,
  };
};
