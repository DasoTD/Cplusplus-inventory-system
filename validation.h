#ifndef VALIDATION_H
#define VALIDATION_H

#include <drogon/drogon.h>

/**
 * Validates incoming HTTP requests for the inventory system
 * @param req The HTTP request to validate
 * @return nullptr if validation passes, or an error response if validation fails
 */
drogon::HttpResponsePtr validateProductRequest(const drogon::HttpRequestPtr& req);

#endif // VALIDATION_H