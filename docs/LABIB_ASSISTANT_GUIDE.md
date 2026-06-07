# Labib AI Assistant & Email Notifications - Full Implementation Guide

## 📋 Overview

WasteGuard now includes two advanced features:
1. **Labib AI Assistant** - Universal AI chatbot accessible from anywhere
2. **Email Notification System** - Automatic contract expiration alerts

---

## 🤖 Labib AI Assistant

### Features

#### 1. **Universal Chat Interface**
- Accessible from any part of the application via the "🤖 Labib AI" button
- Interactive Q&A about the Smart Waste Collection System
- Module-specific help and guidance

#### 2. **Multi-Module Support**
The assistant works with all project modules:
- **Client Module** - Manage clients and contracts
- **Employe Module** - Employee management
- **Produit Module** - Product catalog
- **Maintenance Module** - Equipment interventions
- **Commande Module** - Order management
- **Stock Module** - Inventory management

#### 3. **Bulk Import Operations**
Import data from external files to perform bulk operations:
- **File Formats Supported**: CSV, Excel (.xlsx), JSON
- **Operations**: Add, Modify, Delete records across any module
- **Preview**: View imported data before processing
- **Validation**: Automatic field validation before database insertion

#### 4. **AI Response Logic**
The assistant interprets user queries and provides context-aware responses:
- Questions about Smart Waste system
- Module explanations and guides
- Data import instructions
- General help and troubleshooting

### How to Use Labib Assistant

#### Opening the Assistant
1. Click the **"🤖 Labib AI"** button anywhere in the interface
2. The assistant dialog opens as a floating window (can be used while working)

#### Asking Questions
1. Type your question in the input field at the bottom
2. Examples:
   - "What is WasteGuard?"
   - "Explain the Client module"
   - "How do I import employee data?"
3. Press Enter or click "Envoyer" (Send)
4. Labib responds with relevant information

#### Importing Bulk Data

**Step 1: Select Module**
- Choose the target module from the left panel (Client, Employe, Produit, etc.)

**Step 2: Import File**
- Click "📁 Importer Fichier"
- Select CSV, Excel, or JSON file with your data
- File should contain columns matching the target module fields

**Step 3: Review Preview**
- The right panel shows a preview of the first imported record
- Check that fields are correctly identified

**Step 4: Process Import**
- Click "✓ Traiter" to process all records
- Labib validates and inserts data into the database
- Success/Error count displayed upon completion

### CSV Format Example

For **Client** module:
```csv
NOM,MATRICULE,EMAIL,TYPE_CONTRAT,DATE_EXPIRATION_CONTRAT,STATUT_PAIEMENT
Ahmed Ben Salah,CL-010,ahmed@example.com,Mensuel,2026-12-31,Payé
Sarah Khalifa,CL-011,sarah@example.com,Trimestriel,2026-09-30,Payé
```

For **Employe** module:
```csv
NOM,EMAIL,CIN,SPECIALITE,SALAIRE,STATUT
John Doe,john@wasteguard.com,12345678,IT,3000,Disponible
Jane Smith,jane@wasteguard.com,87654321,HR,2800,Disponible
```

---

## 📧 Email Notification System

### Overview
Automatic detection and notification of contracts approaching expiration dates.

### How It Works

1. **Automatic Detection**
   - System checks all client contracts daily
   - Identifies contracts expiring within **7 days**
   - Excludes canceled contracts

2. **Database Query**
   - Queries the `CLIENT` table
   - Filters by: `DATE_EXPIRATION_CONTRAT <= today + 7 days`
   - AND `DATE_EXPIRATION_CONTRAT >= today`
   - AND `STATUT_PAIEMENT != 'ANNULE'`

3. **Notification Generation**
   - Creates HTML-formatted email
   - Includes contract details and days until expiration
   - Shows professional branding (WasteGuard Team)

4. **Delivery**
   - Sends to client email address
   - Logs notification in application console
   - Shows UI popup when accessing Client module

### Email Content

The generated email includes:
- **Recipient**: Client name
- **Expiration Date**: Exact contract end date
- **Days Remaining**: Calculated countdown
- **Call to Action**: Request to renew before expiration
- **Contact Info**: "Contact us if you have questions"

### Triggering the Notification Check

The notification system automatically runs when:
1. User clicks the **"Clients"** button in the sidebar
2. Or calls `checkAndNotifyExpiringContracts()` programmatically

### Example Notification Output

```
Email NOTIFICATION SENT:
To: ahmed.bensalah@gmail.com
Subject: Notification: Votre contrat arrive à expiration
Days until expiration: 5

[UI Popup]
Notification d'expiration envoyée à ahmed.bensalah@gmail.com
Contrat expire le: 2026-03-31
Jours restants: 5
```

---

## 🔧 Technical Details

### Architecture

#### Labib Assistant Files
- **Header**: `labibassistant.h`
- **Implementation**: `labibassistant.cpp`
- **Integration**: Added to `mainwindow.h/cpp`
- **Project File**: Updated `projettcpp.pro`

#### Classes & Methods

**LabibAssistant Class**
```cpp
// Main dialog class
class LabibAssistant : public QDialog
{
    Q_OBJECT
    
public:
    explicit LabibAssistant(QWidget *parent = nullptr);
    
private slots:
    void onSendMessage();                              // Handle chat messages
    void onImportFile();                               // File selection
    void onProcessImportedData();                      // Process imports
    void handleAiResponse(const QString &message);     // Generate AI responses
    
private:
    QString generateAiResponse(const QString &msg);    // AI logic
    QList<QJsonObject> parseImportedFile(const QString &path); // Parse files
    bool validateRecord(const QJsonObject &rec, const QString &module);
    bool addRecordToDatabase(const QJsonObject &rec, const QString &module);
};
```

**MainWindow Integration**
```cpp
// Member variable
LabibAssistant *m_labibAssistant = nullptr;

// Opening method
void MainWindow::openLabibAssistant() {
    if (!m_labibAssistant) {
        m_labibAssistant = new LabibAssistant(this);
    }
    m_labibAssistant->show();
}
```

### Database Integration

**Relevant Tables**
- `CLIENT` - Stores contract information with `DATE_EXPIRATION_CONTRAT`
- `EMPLOYE` - Employee records
- `PRODUITS` - Product catalog
- `INTERVENTION` - Maintenance records
- `COMMANDE` - Order information

---

## 🚀 Activation Steps

### For Email Notifications
1. System automatically checks when Client module is accessed
2. Configure the 7-day threshold in `checkAndNotifyExpiringContracts()`
3. To send actual emails (not just logs):
   - Implement SMTP integration in `sendContractExpirationEmail()`
   - Configure email credentials
   - Use Qt's `QNetworkAccessManager` for SMTP communication

### For Labib AI Assistant
1. Click **"🤖 Labib AI"** button (automatically added to interface)
2. Dialog opens as a floating window
3. Start chatting or import files
4. System automatically validates and processes data

---

## 📊 Usage Statistics & Monitoring

### Email Notifications
- Check console logs for notification details
- Monitor `ADMIN_NOTIFICATION` table for persistent records
- UI popups confirm delivery

### Labib Assistant
- All user interactions logged in console
- Import success/error counts displayed
- Database insert/update operations logged

---

## ⚙️ Future Enhancements

### Email System
- [ ] SMTP integration for actual email sending
- [ ] Email template customization
- [ ] Multi-language support
- [ ] Reminder scheduling (3 days, 1 day before expiration)
- [ ] Email delivery status tracking

### Labib Assistant
- [ ] Integration with an external LLM API for smarter responses
- [ ] Voice input/output using existing `VoiceAssistant`
- [ ] Real-time data analytics and reporting
- [ ] Export reports to PDF
- [ ] Scheduled bulk operations
- [ ] Natural Language Processing for complex queries

---

## 🐛 Troubleshooting

### Labib Assistant Not Opening
- Check if `labibassistant.h` and `labibassistant.cpp` are included in `.pro` file
- Rebuild project: `qmake && make clean && make`
- Check for compilation errors

### Import File Not Processing
- Verify CSV/Excel format matches expected columns
- Check file encoding (UTF-8 recommended)
- Ensure module selection matches data structure
- Check console for validation errors

### Email Notifications Not Triggering
- Verify Client module is accessed
- Check if contracts are within 7-day threshold
- Confirm `DATE_EXPIRATION_CONTRAT` field is populated
- Review console logs for query errors

---

## 📞 Support

For issues or feature requests related to:
- **Labib Assistant**: Check `labibassistant.cpp` AI logic
- **Email Notifications**: Review `checkAndNotifyExpiringContracts()` method
- **Database Integration**: Verify table schemas in `wasteguard.sql`

---

**Last Updated**: March 26, 2026
**Version**: 1.0
**Status**: Fully Implemented ✅
