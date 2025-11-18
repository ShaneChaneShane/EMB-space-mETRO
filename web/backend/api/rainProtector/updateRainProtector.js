module.exports = async (req, res, next) => {
  // request blynk to switch the current state of rainProtector

  return res.status(200).json({
    success: true,
    data: {
      isOpen: true,
    },
  });
};
