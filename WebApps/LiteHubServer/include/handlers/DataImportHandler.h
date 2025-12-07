#pragma once
#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../LiteHubServer.h"
#include "../../../HttpServer/include/utils/JsonUtil.h"
#include <vector>
#include <string>
#include <map>

// 数据导入结果报告
struct ImportReport {
    bool success;
    int totalRows;
    int successRows;
    int failedRows;
    std::vector<std::string> errors;
    std::vector<std::map<std::string, std::string>> failedData;
    std::string message;
};

// 数据校验结果
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    int rowIndex;
};

class DataImportHandler : public http::router::RouterHandler 
{
public:
    explicit DataImportHandler(LiteHubServer* server) : server_(server) {}
    
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    // 文件上传处理
    bool handleFileUpload(const http::HttpRequest& req, std::string& fileContent, std::string& fileType);
    
    // 解析CSV文件
    std::vector<std::vector<std::string>> parseCSV(const std::string& content);
    
    // 解析Excel文件（简化版，实际应该使用库如libxl）
    std::vector<std::vector<std::string>> parseExcel(const std::string& content);
    
    // 数据校验（等价类和边界值测试）
    ValidationResult validateRow(const std::vector<std::string>& row, int rowIndex, const std::vector<std::string>& headers);
    
    // 校验单个字段
    bool validateField(const std::string& fieldName, const std::string& value, std::string& errorMsg);
    
    // 写入数据库（带事务和回滚）
    bool writeToDatabase(const std::vector<std::vector<std::string>>& validData, 
                        const std::vector<std::string>& headers,
                        ImportReport& report);
    
    // 生成导入报告
    json generateReport(const ImportReport& report);
    
    // 检查文件格式和大小
    bool validateFileFormat(const std::string& filename, size_t fileSize, std::string& errorMsg);
    
    // 边界值测试：检查行数、列数
    bool validateFileAttributes(size_t rowCount, size_t colCount, std::string& errorMsg);

private:
    LiteHubServer*       server_;
    http::MysqlUtil     mysqlUtil_;
    
    // 配置常量
    static const size_t MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB
    static const size_t MAX_ROWS = 10000;
    static const size_t MIN_ROWS = 1;
    static const size_t MAX_COLS = 50;
    static const size_t MIN_COLS = 1;
};

