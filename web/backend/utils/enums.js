// rain states
const RainStatus = Object.freeze({
  RAINING: "raining",
  NO_RAIN: "no_rain",
});

// sunlight levels
const SunlightLevel = Object.freeze({
  VERY_HIGH: "very_high",
  HIGH: "high",
  MODERATE: "moderate",
  LOW: "low",
  NO_SUN: "no_sun",
});

// dryness levels (1–5)
const DrynessLevel = Object.freeze({
  MIN: 1,
  MAX: 5,
});

module.exports = {
  RainStatus,
  SunlightLevel,
  DrynessLevel,
};
