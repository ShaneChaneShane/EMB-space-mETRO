function errorHandler(err, req, res, next) {
  // log it for internal debugging
  console.error(err.stack);

  const statusCode = err.statusCode || 500;
  const errorMessage = err.message || "Internal Server Error";

  res.status(statusCode).json({
    success: false,
    message: errorMessage,
  });
}

module.exports = errorHandler;
