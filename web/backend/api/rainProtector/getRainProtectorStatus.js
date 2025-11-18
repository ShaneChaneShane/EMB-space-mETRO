module.exports = async (req, res, next) => {
  // request current rainProtector status from blynk

  return res.status(200).json({
    success: true,
    data: {
      isOpen: true,
    },
  });
};
