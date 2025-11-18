module.exports = async (req, res, next) => {
  return res.status(200).json({
    success: true,
    data: {
      test: "test",
    },
  });
};
