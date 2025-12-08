const { DrynessLevel, SunlightLevel, RainStatus } = require("./enums");

module.exports = function evaluateSensorData(
  humidityEnv,
  humidityClothes,
  rain,
  temperature,
  light
) {
  const rainStatus = rain == 1 ? RainStatus.RAINING : RainStatus.NO_RAIN;

  const sunlightLevel =
    light > 3685 // 90% of 4095
      ? SunlightLevel.VERY_HIGH
      : light > 3276 // 80% of 4095
      ? SunlightLevel.HIGH
      : light > 1228 // 30% of 4095
      ? SunlightLevel.MODERATE
      : SunlightLevel.LOW;
  const humidityDiff = humidityClothes - humidityEnv;

  const step = (DrynessLevel.MAX - DrynessLevel.MIN) / 5;

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
