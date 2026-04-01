with open('mainwindow.h', 'r', encoding='utf-8', errors='surrogateescape') as f:
    content = f.read()

target = '    void setupEmployeModule();\n\n    // Client Card View implementation'
replacement = '''    void setupEmployeModule();

    // Stock Module
    void setupStockModule();
    void setupStockTableData();
    void applyStockFilterAndSort();
    void buildStockStats();
    void checkStockNotifications();
    void showStockAlertsDialog();
    void on_btnSmartCart_clicked();

    // Stock Card View implementation
    void setupStockCardViewContainer();
    void refreshStockCardView();
    QWidget* createStockCard(int row);

    // Client Card View implementation'''

if target in content:
    content = content.replace(target, replacement, 1)
    with open('mainwindow.h', 'w', encoding='utf-8', errors='surrogateescape') as f:
        f.write(content)
    print("mainwindow.h patched successfully.")
else:
    print("Target not found!")
    # debug
    idx = content.find('setupEmployeModule')
    print(repr(content[idx-5:idx+150]))
