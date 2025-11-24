const { DrynessLevel, SunlightLevel, RainStatus } = require("./enums");

module.exports = function evaluateSensorData(
  humidityEnv,
  humidityClothes,
  rain,
  temperature,
  light
) {
  const rainStatus = rain == 1 ? RainStatus.RAINING : RainStatus.NO_RAIN;

  // SOME IMAGINARY LOGIC TO DETERMINE SUNLIGHT LEVEL
  const sunlightLevel =
    light > 2850 // 75% of 4095
      ? SunlightLevel.VERY_HIGH
      : light > 2648 // 65% of 4095
      ? SunlightLevel.HIGH
      : light > 1630 // 40% of 4095
      ? SunlightLevel.MODERATE
      : SunlightLevel.LOW;
  const humidityDiff = humidityClothes - humidityEnv;

  const step = (DrynessLevel.MAX - DrynessLevel.MIN) / 5;

  // SOME IMAGINARY LOGIC TO DETERMINE DRYNESS LEVEL
  const drynessLevel =
    humidityDiff > 30
      ? DrynessLevel.MIN
      : humidityDiff > 25
      ? DrynessLevel.MIN + step
      : humidityDiff > 20
      ? DrynessLevel.MIN + 2 * step
      : humidityDiff > 15
      ? DrynessLevel.MIN + 3 * step
      : humidityDiff > 10
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
