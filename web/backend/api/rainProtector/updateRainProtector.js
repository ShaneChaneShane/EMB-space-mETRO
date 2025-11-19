module.exports = async (req, res, next) => {
  // TO-DO: request blynk to switch the current state of rainProtector
  // TO-DO: error handling: when rain protector can't be switched
  return res.status(200).json({
    success: true,
    data: {
      isOpen: true,
    },
  });
};
