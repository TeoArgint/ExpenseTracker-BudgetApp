#import <AppKit/AppKit.h>

#include "app/AppSupport.h"
#include "repo/FileBudgetStateRepository.h"
#include "repo/FileExpenseRepository.h"
#include "service/ExpenseService.h"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

using AppSupport::MonthlyPoint;

static NSString* ToNSString(const std::string& value) {
    return [NSString stringWithUTF8String:value.c_str()];
}

static std::string ToStdString(NSString* value) {
    return value ? std::string(value.UTF8String) : std::string();
}

static NSTextField* MakeLabel(NSString* text, CGFloat fontSize = 13, BOOL bold = NO) {
    NSTextField* label = [[NSTextField alloc] initWithFrame:NSZeroRect];
    label.translatesAutoresizingMaskIntoConstraints = NO;
    label.stringValue = text ?: @"";
    label.editable = NO;
    label.bezeled = NO;
    label.drawsBackground = NO;
    label.selectable = NO;
    label.lineBreakMode = NSLineBreakByWordWrapping;
    label.usesSingleLineMode = NO;
    label.font = bold ? [NSFont boldSystemFontOfSize:fontSize] : [NSFont systemFontOfSize:fontSize];
    label.textColor = [NSColor colorWithCalibratedRed:0.07 green:0.11 blue:0.20 alpha:1.0];
    return label;
}

static NSTextField* MakeInput(NSString* placeholder) {
    NSTextField* input = [[NSTextField alloc] initWithFrame:NSZeroRect];
    input.translatesAutoresizingMaskIntoConstraints = NO;
    input.placeholderString = placeholder;
    input.textColor = [NSColor colorWithCalibratedRed:0.08 green:0.12 blue:0.18 alpha:1.0];
    input.backgroundColor = [NSColor colorWithCalibratedRed:0.98 green:0.99 blue:1.0 alpha:1.0];
    input.bordered = YES;
    input.bezelStyle = NSTextFieldRoundedBezel;
    return input;
}

static NSButton* MakeButton(NSString* title, id target, SEL action) {
    NSButton* button = [NSButton buttonWithTitle:title target:target action:action];
    button.translatesAutoresizingMaskIntoConstraints = NO;
    button.bezelStyle = NSBezelStyleRounded;
    button.contentTintColor = [NSColor colorWithCalibratedRed:0.05 green:0.28 blue:0.56 alpha:1.0];
    return button;
}

static NSStackView* MakeVStack(CGFloat spacing) {
    NSStackView* stack = [[NSStackView alloc] initWithFrame:NSZeroRect];
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    stack.orientation = NSUserInterfaceLayoutOrientationVertical;
    stack.spacing = spacing;
    stack.alignment = NSLayoutAttributeLeading;
    stack.distribution = NSStackViewDistributionFill;
    return stack;
}

static NSStackView* MakeHStack(CGFloat spacing) {
    NSStackView* stack = [[NSStackView alloc] initWithFrame:NSZeroRect];
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    stack.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    stack.spacing = spacing;
    stack.alignment = NSLayoutAttributeTop;
    return stack;
}

static NSBox* MakeSection(NSString* title) {
    NSBox* box = [[NSBox alloc] initWithFrame:NSZeroRect];
    box.translatesAutoresizingMaskIntoConstraints = NO;
    box.title = title;
    box.boxType = NSBoxPrimary;
    box.fillColor = [NSColor colorWithCalibratedRed:0.93 green:0.96 blue:0.99 alpha:1.0];
    box.borderColor = [NSColor colorWithCalibratedRed:0.53 green:0.67 blue:0.82 alpha:1.0];
    box.borderWidth = 1.0;
    box.cornerRadius = 14.0;
    box.titleFont = [NSFont boldSystemFontOfSize:13];
    return box;
}

static NSView* MakeFieldRow(NSString* title, NSView* field) {
    NSStackView* row = MakeHStack(6);
    row.distribution = NSStackViewDistributionFill;
    NSTextField* label = MakeLabel(title, 10, YES);
    [label.widthAnchor constraintEqualToConstant:82].active = YES;
    [row addArrangedSubview:label];
    [row addArrangedSubview:field];
    return row;
}

@interface MonthlyChartView : NSView {
    std::vector<MonthlyPoint> _points;
    NSInteger _selectedIndex;
}
- (void)setPoints:(const std::vector<MonthlyPoint>&)points selectedIndex:(NSInteger)selectedIndex;
@end

@implementation MonthlyChartView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        self.translatesAutoresizingMaskIntoConstraints = NO;
        [self.heightAnchor constraintGreaterThanOrEqualToConstant:110].active = YES;
    }
    return self;
}

- (BOOL)isFlipped {
    return YES;
}

- (void)setPoints:(const std::vector<MonthlyPoint>&)points selectedIndex:(NSInteger)selectedIndex {
    _points = points;
    _selectedIndex = selectedIndex;
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];

    NSRect bounds = NSInsetRect(self.bounds, 6, 6);
    [[NSColor colorWithCalibratedRed:0.89 green:0.94 blue:0.99 alpha:1.0] setFill];
    [[NSBezierPath bezierPathWithRoundedRect:bounds xRadius:20 yRadius:20] fill];

    if (_points.empty()) {
        [@"No monthly expenses yet" drawAtPoint:NSMakePoint(18, 24) withAttributes:@{
            NSFontAttributeName: [NSFont systemFontOfSize:12],
            NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.23 green:0.32 blue:0.43 alpha:1.0]
        }];
        return;
    }

    double maxValue = 1;
    for (const auto& point : _points) {
        maxValue = std::max(maxValue, point.total);
    }

    const CGFloat topPadding = 16;
    const CGFloat bottomPadding = 36;
    const CGFloat leftPadding = 12;
    const CGFloat rightPadding = 10;
    const CGFloat usableHeight = bounds.size.height - topPadding - bottomPadding;
    const CGFloat usableWidth = bounds.size.width - leftPadding - rightPadding;
    const CGFloat slotWidth = usableWidth / std::max<CGFloat>(1.0, static_cast<CGFloat>(_points.size()));
    const CGFloat barWidth = std::max<CGFloat>(16.0, slotWidth - 12.0);

    [[NSColor colorWithCalibratedRed:0.50 green:0.64 blue:0.79 alpha:1.0] setStroke];
    for (int i = 0; i < 4; ++i) {
        const CGFloat y = topPadding + usableHeight * (static_cast<CGFloat>(i) / 3.0);
        NSBezierPath* line = [NSBezierPath bezierPath];
        [line moveToPoint:NSMakePoint(leftPadding, y)];
        [line lineToPoint:NSMakePoint(bounds.size.width - rightPadding, y)];
        line.lineWidth = 1;
        [line stroke];
    }

    NSDictionary* valueAttrs = @{
        NSFontAttributeName: [NSFont boldSystemFontOfSize:9],
        NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.06 green:0.10 blue:0.18 alpha:1.0]
    };
    NSDictionary* labelAttrs = @{
        NSFontAttributeName: [NSFont systemFontOfSize:9],
        NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.22 green:0.31 blue:0.43 alpha:1.0]
    };

    for (std::size_t i = 0; i < _points.size(); ++i) {
        const auto& point = _points[i];
        const CGFloat x = leftPadding + slotWidth * i + (slotWidth - barWidth) / 2.0;
        const CGFloat ratio = static_cast<CGFloat>(point.total / maxValue);
        const CGFloat height = std::max<CGFloat>(10.0, usableHeight * ratio);
        const CGFloat y = topPadding + usableHeight - height;
        const NSRect barRect = NSMakeRect(x, y, barWidth, height);

        NSColor* from = (static_cast<NSInteger>(i) == _selectedIndex)
            ? [NSColor colorWithCalibratedRed:0.10 green:0.48 blue:0.93 alpha:1.0]
            : [NSColor colorWithCalibratedRed:0.48 green:0.73 blue:0.98 alpha:1.0];
        NSColor* to = (static_cast<NSInteger>(i) == _selectedIndex)
            ? [NSColor colorWithCalibratedRed:0.02 green:0.26 blue:0.69 alpha:1.0]
            : [NSColor colorWithCalibratedRed:0.25 green:0.50 blue:0.90 alpha:1.0];
        NSGradient* gradient = [[NSGradient alloc] initWithStartingColor:from endingColor:to];
        [NSGraphicsContext saveGraphicsState];
        [[NSBezierPath bezierPathWithRoundedRect:barRect xRadius:10 yRadius:10] addClip];
        [gradient drawInRect:barRect angle:90];
        [NSGraphicsContext restoreGraphicsState];

        std::ostringstream amount;
        amount << static_cast<int>(std::round(point.total));
        [ToNSString(amount.str()) drawAtPoint:NSMakePoint(x, std::max<CGFloat>(4.0, y - 18.0)) withAttributes:valueAttrs];
        [ToNSString(point.label) drawAtPoint:NSMakePoint(x - 2, topPadding + usableHeight + 6) withAttributes:labelAttrs];
    }
}

@end

@interface ExpenseAppController : NSObject <NSApplicationDelegate, NSTableViewDataSource, NSTableViewDelegate> {
    std::unique_ptr<FileExpenseRepository> _expenseRepo;
    std::unique_ptr<FileBudgetStateRepository> _stateRepo;
    std::unique_ptr<ExpenseService> _service;
    std::vector<Expense> _monthExpenses;
    std::vector<MonthlyPoint> _availableMonths;
    std::optional<int> _selectedExpenseId;

    NSWindow* _window;
    NSPopUpButton* _monthPicker;
    NSPopUpButton* _goalPicker;

    NSTextField* _moneyLeftLabel;
    NSTextField* _savedLabel;
    NSTextField* _spentMonthLabel;
    NSTextField* _billsLabel;
    NSTextField* _perDayLabel;

    NSTextField* _salaryField;
    NSTextField* _monthlySavingField;
    NSTextField* _fixedBillNameField;
    NSTextField* _fixedBillAmountField;
    NSTextField* _quickSaveField;
    NSTextField* _goalNameField;
    NSTextField* _goalAmountField;
    NSTextField* _goalEstimateField;

    NSTextField* _expenseAmountField;
    NSPopUpButton* _expenseCategoryPopup;
    NSTextField* _expenseDateField;
    NSTextField* _expenseNoteField;
    NSTextField* _selectionHintLabel;

    NSStackView* _fixedBillsStack;
    NSStackView* _goalsStack;
    NSStackView* _ideasStack;
    MonthlyChartView* _chartView;
    NSTableView* _tableView;
}
@end

@implementation ExpenseAppController

- (instancetype)init {
    self = [super init];
    if (self) {
        const std::string dataDir = APP_DATA_DIR;
        std::filesystem::create_directories(dataDir);
        _expenseRepo = std::make_unique<FileExpenseRepository>(dataDir + "/expense.csv");
        _stateRepo = std::make_unique<FileBudgetStateRepository>(dataDir + "/budget_state.txt");
        _service = std::make_unique<ExpenseService>(*_expenseRepo, *_stateRepo);
        AppSupport::ensureDemoDataIfNeeded(*_service);
        const auto [year, month] = AppSupport::currentYearMonth();
        const auto state = _service->getState();
        _service->initializeIfNeeded(year, month, state.getYear() == 0 ? 5200 : state.getSalary());
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    (void)notification;
    [self buildWindow];
    [self refreshUI];
    [_window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    (void)sender;
    return YES;
}

- (std::pair<int, int>)selectedMonth {
    if (_availableMonths.empty() || _monthPicker.indexOfSelectedItem < 0) {
        return AppSupport::currentYearMonth();
    }
    const MonthlyPoint& point = _availableMonths[static_cast<std::size_t>(_monthPicker.indexOfSelectedItem)];
    return {point.year, point.month};
}

- (void)showError:(const std::exception&)error {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = @"Please check this";
    alert.informativeText = ToNSString(error.what());
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (void)showGoalNotification:(const SavingsGoal&)goal {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = @"Savings goal reached";
    alert.informativeText = [NSString stringWithFormat:@"You reached \"%@\". Nice work, you now have enough saved for this goal.", ToNSString(goal.name)];
    [alert addButtonWithTitle:@"Great"];
    [alert runModal];
}

- (void)checkGoalNotifications {
    const auto state = _service->getState();
    for (const auto& goal : state.getGoals()) {
        if (!goal.notified && state.getSavings() >= goal.target) {
            [self showGoalNotification:goal];
            _service->markGoalNotified(goal.name, true);
        }
    }
}

- (void)buildWindow {
    _window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 920, 590)
                                          styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                     NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable)
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    _window.title = @"Budget Planner";
    [_window center];
    [_window setMinSize:NSMakeSize(640, 410)];

    NSView* content = _window.contentView;
    [content setWantsLayer:YES];
    content.layer.backgroundColor = [NSColor colorWithCalibratedRed:0.82 green:0.89 blue:0.96 alpha:1.0].CGColor;

    NSScrollView* mainScroll = [[NSScrollView alloc] initWithFrame:NSZeroRect];
    mainScroll.translatesAutoresizingMaskIntoConstraints = NO;
    mainScroll.hasVerticalScroller = YES;
    mainScroll.hasHorizontalScroller = NO;
    mainScroll.drawsBackground = NO;
    [content addSubview:mainScroll];
    [NSLayoutConstraint activateConstraints:@[
        [mainScroll.topAnchor constraintEqualToAnchor:content.topAnchor],
        [mainScroll.leadingAnchor constraintEqualToAnchor:content.leadingAnchor],
        [mainScroll.trailingAnchor constraintEqualToAnchor:content.trailingAnchor],
        [mainScroll.bottomAnchor constraintEqualToAnchor:content.bottomAnchor]
    ]];

    NSView* canvas = [[NSView alloc] initWithFrame:NSZeroRect];
    canvas.translatesAutoresizingMaskIntoConstraints = NO;
    mainScroll.documentView = canvas;

    NSStackView* root = MakeVStack(14);
    root.edgeInsets = NSEdgeInsetsMake(10, 10, 10, 10);
    [canvas addSubview:root];
    [NSLayoutConstraint activateConstraints:@[
        [canvas.widthAnchor constraintEqualToAnchor:mainScroll.contentView.widthAnchor],
        [canvas.heightAnchor constraintGreaterThanOrEqualToAnchor:mainScroll.contentView.heightAnchor],
        [root.topAnchor constraintEqualToAnchor:canvas.topAnchor],
        [root.leadingAnchor constraintEqualToAnchor:canvas.leadingAnchor],
        [root.trailingAnchor constraintEqualToAnchor:canvas.trailingAnchor],
        [root.bottomAnchor constraintEqualToAnchor:canvas.bottomAnchor]
    ]];

    NSStackView* header = MakeHStack(12);
    header.distribution = NSStackViewDistributionFill;
    NSStackView* titleStack = MakeVStack(3);
    NSTextField* title = MakeLabel(@"Budget Planner", 19, YES);
    NSTextField* subtitle = MakeLabel(@"Salary, savings, goals, past months.", 11, NO);
    title.textColor = [NSColor colorWithCalibratedRed:0.07 green:0.16 blue:0.31 alpha:1.0];
    subtitle.textColor = [NSColor colorWithCalibratedRed:0.20 green:0.31 blue:0.45 alpha:1.0];
    [titleStack addArrangedSubview:title];
    [titleStack addArrangedSubview:subtitle];

    _monthPicker = [[NSPopUpButton alloc] initWithFrame:NSZeroRect];
    _monthPicker.translatesAutoresizingMaskIntoConstraints = NO;
    _monthPicker.target = self;
    _monthPicker.action = @selector(changeMonth:);
    [_monthPicker.widthAnchor constraintEqualToConstant:190].active = YES;
    NSStackView* monthStack = MakeVStack(4);
    [monthStack addArrangedSubview:MakeLabel(@"Open month", 12, YES)];
    [monthStack addArrangedSubview:_monthPicker];

    [header addArrangedSubview:titleStack];
    [header addArrangedSubview:monthStack];
    [header setContentHuggingPriority:NSLayoutPriorityRequired forOrientation:NSLayoutConstraintOrientationVertical];
    [root addArrangedSubview:header];

    NSStackView* cards = MakeHStack(10);
    cards.distribution = NSStackViewDistributionFillEqually;
    NSArray<NSString*>* titles = @[@"Money left", @"Saved", @"Spent this month", @"Fixed bills", @"Safe per day"];
    NSMutableArray<NSTextField*>* valueRefs = [NSMutableArray array];
    for (NSString* cardTitle in titles) {
        NSBox* card = MakeSection(cardTitle);
        NSTextField* value = MakeLabel(@"0 lei", 24, YES);
        NSTextField* note = MakeLabel(@"Kept between runs", 11, NO);
        value.textColor = [NSColor colorWithCalibratedRed:0.05 green:0.28 blue:0.56 alpha:1.0];
        note.textColor = [NSColor colorWithCalibratedRed:0.28 green:0.40 blue:0.54 alpha:1.0];
        NSStackView* stack = MakeVStack(4);
        [stack addArrangedSubview:value];
        [stack addArrangedSubview:note];
        [card.contentView addSubview:stack];
        [NSLayoutConstraint activateConstraints:@[
            [stack.topAnchor constraintEqualToAnchor:card.contentView.topAnchor constant:14],
            [stack.leadingAnchor constraintEqualToAnchor:card.contentView.leadingAnchor constant:14],
            [stack.trailingAnchor constraintEqualToAnchor:card.contentView.trailingAnchor constant:-14],
            [stack.bottomAnchor constraintEqualToAnchor:card.contentView.bottomAnchor constant:-14],
            [card.heightAnchor constraintEqualToConstant:54]
        ]];
        [cards addArrangedSubview:card];
        [valueRefs addObject:value];
    }
    _moneyLeftLabel = valueRefs[0];
    _savedLabel = valueRefs[1];
    _spentMonthLabel = valueRefs[2];
    _billsLabel = valueRefs[3];
    _perDayLabel = valueRefs[4];
    [cards setContentHuggingPriority:NSLayoutPriorityRequired forOrientation:NSLayoutConstraintOrientationVertical];
    [cards setContentCompressionResistancePriority:NSLayoutPriorityRequired forOrientation:NSLayoutConstraintOrientationVertical];
    [root addArrangedSubview:cards];

    NSStackView* topRow = MakeHStack(12);
    topRow.distribution = NSStackViewDistributionFillEqually;
    [topRow setContentHuggingPriority:NSLayoutPriorityDefaultHigh forOrientation:NSLayoutConstraintOrientationVertical];
    [topRow setContentCompressionResistancePriority:NSLayoutPriorityRequired forOrientation:NSLayoutConstraintOrientationVertical];

    NSBox* expenseBox = MakeSection(@"Expenses");
    NSStackView* expenseStack = MakeVStack(9);
    _expenseAmountField = MakeInput(@"120");
    _expenseCategoryPopup = [[NSPopUpButton alloc] initWithFrame:NSZeroRect];
    _expenseCategoryPopup.translatesAutoresizingMaskIntoConstraints = NO;
    [_expenseCategoryPopup addItemsWithTitles:@[@"Groceries", @"Transport", @"Home", @"Fun", @"Health", @"Coffee", @"Other"]];
    _expenseDateField = MakeInput(@"YYYY-MM-DD");
    _expenseNoteField = MakeInput(@"What did you buy?");
    _selectionHintLabel = MakeLabel(@"Select a row below if you want to update or delete it.", 12, NO);
    _selectionHintLabel.textColor = [NSColor colorWithCalibratedRed:0.28 green:0.40 blue:0.54 alpha:1.0];
    [expenseStack addArrangedSubview:MakeFieldRow(@"Amount", _expenseAmountField)];
    [expenseStack addArrangedSubview:MakeFieldRow(@"Category", _expenseCategoryPopup)];
    [expenseStack addArrangedSubview:MakeFieldRow(@"Date", _expenseDateField)];
    [expenseStack addArrangedSubview:MakeFieldRow(@"Note", _expenseNoteField)];
    NSStackView* expenseButtons = MakeHStack(8);
    expenseButtons.distribution = NSStackViewDistributionFillEqually;
    [expenseButtons addArrangedSubview:MakeButton(@"Add expense", self, @selector(addExpense:))];
    [expenseButtons addArrangedSubview:MakeButton(@"Update selected", self, @selector(updateExpense:))];
    [expenseButtons addArrangedSubview:MakeButton(@"Delete selected", self, @selector(deleteExpense:))];
    [expenseStack addArrangedSubview:expenseButtons];
    [expenseStack addArrangedSubview:_selectionHintLabel];
    [expenseBox.contentView addSubview:expenseStack];
    [NSLayoutConstraint activateConstraints:@[
        [expenseStack.topAnchor constraintEqualToAnchor:expenseBox.contentView.topAnchor constant:14],
        [expenseStack.leadingAnchor constraintEqualToAnchor:expenseBox.contentView.leadingAnchor constant:14],
        [expenseStack.trailingAnchor constraintEqualToAnchor:expenseBox.contentView.trailingAnchor constant:-14],
        [expenseStack.bottomAnchor constraintEqualToAnchor:expenseBox.contentView.bottomAnchor constant:-14]
    ]];

    NSBox* planBox = MakeSection(@"Salary, saving and bills");
    NSStackView* planStack = MakeVStack(8);
    _salaryField = MakeInput(@"5200");
    _monthlySavingField = MakeInput(@"650");
    _fixedBillNameField = MakeInput(@"Electricity");
    _fixedBillAmountField = MakeInput(@"180");
    _fixedBillsStack = MakeVStack(4);
    [planStack addArrangedSubview:MakeFieldRow(@"Salary", _salaryField)];
    [planStack addArrangedSubview:MakeButton(@"Add / update salary", self, @selector(saveSalary:))];
    [planStack addArrangedSubview:MakeFieldRow(@"Auto-save", _monthlySavingField)];
    [planStack addArrangedSubview:MakeButton(@"Add / update save amount", self, @selector(saveAutoSaving:))];
    [planStack addArrangedSubview:MakeFieldRow(@"Bill name", _fixedBillNameField)];
    [planStack addArrangedSubview:MakeFieldRow(@"Bill amount", _fixedBillAmountField)];
    NSStackView* billButtons = MakeHStack(8);
    billButtons.distribution = NSStackViewDistributionFillEqually;
    [billButtons addArrangedSubview:MakeButton(@"Add bill", self, @selector(addFixedBill:))];
    [billButtons addArrangedSubview:MakeButton(@"Remove bill", self, @selector(removeFixedBill:))];
    [planStack addArrangedSubview:billButtons];
    [planStack addArrangedSubview:MakeLabel(@"Saved bills", 12, YES)];
    [planStack addArrangedSubview:_fixedBillsStack];
    [planBox.contentView addSubview:planStack];
    [NSLayoutConstraint activateConstraints:@[
        [planStack.topAnchor constraintEqualToAnchor:planBox.contentView.topAnchor constant:14],
        [planStack.leadingAnchor constraintEqualToAnchor:planBox.contentView.leadingAnchor constant:14],
        [planStack.trailingAnchor constraintEqualToAnchor:planBox.contentView.trailingAnchor constant:-14],
        [planStack.bottomAnchor constraintEqualToAnchor:planBox.contentView.bottomAnchor constant:-14]
    ]];

    NSBox* goalBox = MakeSection(@"Saving goals");
    NSStackView* goalStack = MakeVStack(8);
    _goalNameField = MakeInput(@"New laptop");
    _goalAmountField = MakeInput(@"3000");
    _goalEstimateField = MakeLabel(@"When your savings reach a goal, the app will notify you once and keep it saved.", 12, NO);
    _goalEstimateField.textColor = [NSColor colorWithCalibratedRed:0.28 green:0.40 blue:0.54 alpha:1.0];
    _quickSaveField = MakeInput(@"100");
    _goalPicker = [[NSPopUpButton alloc] initWithFrame:NSZeroRect];
    _goalPicker.translatesAutoresizingMaskIntoConstraints = NO;
    _goalsStack = MakeVStack(4);
    [goalStack addArrangedSubview:MakeFieldRow(@"Goal name", _goalNameField)];
    [goalStack addArrangedSubview:MakeFieldRow(@"Goal amount", _goalAmountField)];
    NSStackView* goalButtons = MakeHStack(8);
    goalButtons.distribution = NSStackViewDistributionFillEqually;
    [goalButtons addArrangedSubview:MakeButton(@"Add goal", self, @selector(addGoal:))];
    [goalButtons addArrangedSubview:MakeButton(@"Remove chosen goal", self, @selector(removeGoal:))];
    [goalStack addArrangedSubview:goalButtons];
    [goalStack addArrangedSubview:MakeFieldRow(@"Choose goal", _goalPicker)];
    [goalStack addArrangedSubview:MakeButton(@"Estimate selected goal", self, @selector(estimateSelectedGoal:))];
    [goalStack addArrangedSubview:_goalEstimateField];
    [goalStack addArrangedSubview:MakeFieldRow(@"Move to savings", _quickSaveField)];
    [goalStack addArrangedSubview:MakeButton(@"Save money now", self, @selector(quickSave:))];
    [goalStack addArrangedSubview:MakeLabel(@"Goals and savings stay saved for every run of the app.", 12, NO)];
    [goalStack addArrangedSubview:MakeLabel(@"Goal progress", 12, YES)];
    [goalStack addArrangedSubview:_goalsStack];
    [goalBox.contentView addSubview:goalStack];
    [NSLayoutConstraint activateConstraints:@[
        [goalStack.topAnchor constraintEqualToAnchor:goalBox.contentView.topAnchor constant:14],
        [goalStack.leadingAnchor constraintEqualToAnchor:goalBox.contentView.leadingAnchor constant:14],
        [goalStack.trailingAnchor constraintEqualToAnchor:goalBox.contentView.trailingAnchor constant:-14],
        [goalStack.bottomAnchor constraintEqualToAnchor:goalBox.contentView.bottomAnchor constant:-14]
    ]];

    [topRow addArrangedSubview:expenseBox];
    [topRow addArrangedSubview:planBox];
    [topRow addArrangedSubview:goalBox];
    [topRow.heightAnchor constraintGreaterThanOrEqualToConstant:235].active = YES;
    [root addArrangedSubview:topRow];

    NSStackView* middleRow = MakeHStack(12);
    middleRow.distribution = NSStackViewDistributionFill;
    [middleRow setContentHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationVertical];
    [middleRow setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationVertical];

    NSBox* chartBox = MakeSection(@"Monthly chart");
    _chartView = [[MonthlyChartView alloc] initWithFrame:NSZeroRect];
    [chartBox.contentView addSubview:_chartView];
    [NSLayoutConstraint activateConstraints:@[
        [_chartView.topAnchor constraintEqualToAnchor:chartBox.contentView.topAnchor constant:12],
        [_chartView.leadingAnchor constraintEqualToAnchor:chartBox.contentView.leadingAnchor constant:12],
        [_chartView.trailingAnchor constraintEqualToAnchor:chartBox.contentView.trailingAnchor constant:-12],
        [_chartView.bottomAnchor constraintEqualToAnchor:chartBox.contentView.bottomAnchor constant:-12],
        [chartBox.widthAnchor constraintGreaterThanOrEqualToConstant:360]
    ]];

    NSBox* ideasBox = MakeSection(@"Ideas to save more");
    _ideasStack = MakeVStack(6);
    [ideasBox.contentView addSubview:_ideasStack];
    [NSLayoutConstraint activateConstraints:@[
        [_ideasStack.topAnchor constraintEqualToAnchor:ideasBox.contentView.topAnchor constant:14],
        [_ideasStack.leadingAnchor constraintEqualToAnchor:ideasBox.contentView.leadingAnchor constant:14],
        [_ideasStack.trailingAnchor constraintEqualToAnchor:ideasBox.contentView.trailingAnchor constant:-14],
        [_ideasStack.bottomAnchor constraintEqualToAnchor:ideasBox.contentView.bottomAnchor constant:-14],
        [ideasBox.widthAnchor constraintGreaterThanOrEqualToConstant:180]
    ]];

    [middleRow addArrangedSubview:chartBox];
    [middleRow addArrangedSubview:ideasBox];
    [middleRow.heightAnchor constraintGreaterThanOrEqualToConstant:150].active = YES;
    [root addArrangedSubview:middleRow];

    NSBox* tableBox = MakeSection(@"Expenses in the selected month");
    NSScrollView* scrollView = [[NSScrollView alloc] initWithFrame:NSZeroRect];
    scrollView.translatesAutoresizingMaskIntoConstraints = NO;
    scrollView.hasVerticalScroller = YES;
    _tableView = [[NSTableView alloc] initWithFrame:NSZeroRect];
    _tableView.delegate = self;
    _tableView.dataSource = self;
    _tableView.columnAutoresizingStyle = NSTableViewUniformColumnAutoresizingStyle;
    NSArray<NSDictionary*>* columns = @[
        @{@"id": @"date", @"title": @"Date", @"width": @120},
        @{@"id": @"category", @"title": @"Category", @"width": @160},
        @{@"id": @"amount", @"title": @"Amount", @"width": @110},
        @{@"id": @"note", @"title": @"Note", @"width": @700}
    ];
    for (NSDictionary* columnInfo in columns) {
        NSTableColumn* column = [[NSTableColumn alloc] initWithIdentifier:columnInfo[@"id"]];
        column.title = columnInfo[@"title"];
        column.width = [columnInfo[@"width"] doubleValue];
        [_tableView addTableColumn:column];
    }
    scrollView.documentView = _tableView;
    [tableBox.contentView addSubview:scrollView];
    [NSLayoutConstraint activateConstraints:@[
        [scrollView.topAnchor constraintEqualToAnchor:tableBox.contentView.topAnchor constant:12],
        [scrollView.leadingAnchor constraintEqualToAnchor:tableBox.contentView.leadingAnchor constant:12],
        [scrollView.trailingAnchor constraintEqualToAnchor:tableBox.contentView.trailingAnchor constant:-12],
        [scrollView.bottomAnchor constraintEqualToAnchor:tableBox.contentView.bottomAnchor constant:-12],
        [tableBox.heightAnchor constraintGreaterThanOrEqualToConstant:96]
    ]];
    [tableBox setContentHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationVertical];
    [tableBox setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationVertical];
    [scrollView.heightAnchor constraintGreaterThanOrEqualToConstant:70].active = YES;
    [root addArrangedSubview:tableBox];
}

- (void)reloadMonthPickerKeepingSelection {
    NSString* previous = _monthPicker.titleOfSelectedItem;
    _availableMonths = AppSupport::availableMonths(*_service);
    [_monthPicker removeAllItems];
    for (const auto& month : _availableMonths) {
        [_monthPicker addItemWithTitle:ToNSString(month.label)];
    }
    NSInteger index = previous ? [_monthPicker indexOfItemWithTitle:previous] : -1;
    if (index < 0) {
        index = 0;
    }
    if (_monthPicker.numberOfItems > 0) {
        [_monthPicker selectItemAtIndex:index];
    }
}

- (void)rebuildFixedBills:(const BudgetState&)state {
    for (NSView* view in _fixedBillsStack.arrangedSubviews.copy) {
        [_fixedBillsStack removeArrangedSubview:view];
        [view removeFromSuperview];
    }
    if (state.getFixedBills().empty()) {
        [_fixedBillsStack addArrangedSubview:MakeLabel(@"No bills saved yet.", 12, NO)];
        return;
    }
    for (const auto& [name, amount] : state.getFixedBills()) {
        [_fixedBillsStack addArrangedSubview:MakeLabel([NSString stringWithFormat:@"%@  •  %.0f lei", ToNSString(name), amount], 12, NO)];
    }
}

- (void)rebuildGoals:(const BudgetState&)state {
    NSString* previous = _goalPicker.titleOfSelectedItem;
    [_goalPicker removeAllItems];
    for (NSView* view in _goalsStack.arrangedSubviews.copy) {
        [_goalsStack removeArrangedSubview:view];
        [view removeFromSuperview];
    }
    if (state.getGoals().empty()) {
        [_goalPicker addItemWithTitle:@"No goals"];
        [_goalsStack addArrangedSubview:MakeLabel(@"Add a goal so the app can tell you when your savings reach it.", 12, NO)];
        return;
    }
    for (const auto& goal : state.getGoals()) {
        [_goalPicker addItemWithTitle:ToNSString(goal.name)];
        const double progress = std::min(100.0, goal.target > 0 ? (state.getSavings() / goal.target) * 100.0 : 0.0);
        NSString* line = [NSString stringWithFormat:@"%@  •  %.0f / %.0f lei  •  %.0f%%",
                          ToNSString(goal.name), state.getSavings(), goal.target, progress];
        [_goalsStack addArrangedSubview:MakeLabel(line, 12, NO)];
    }
    NSInteger idx = previous ? [_goalPicker indexOfItemWithTitle:previous] : -1;
    if (idx < 0) {
        idx = 0;
    }
    [_goalPicker selectItemAtIndex:idx];
}

- (void)rebuildIdeas:(const std::vector<std::string>&)ideas average:(double)average {
    for (NSView* view in _ideasStack.arrangedSubviews.copy) {
        [_ideasStack removeArrangedSubview:view];
        [view removeFromSuperview];
    }
    [_ideasStack addArrangedSubview:MakeLabel([NSString stringWithFormat:@"Average monthly spending: %.0f lei", average], 13, YES)];
    for (const auto& idea : ideas) {
        [_ideasStack addArrangedSubview:MakeLabel(ToNSString("• " + idea), 12, NO)];
    }
}

- (void)clearExpenseForm {
    _selectedExpenseId.reset();
    _expenseAmountField.stringValue = @"";
    _expenseDateField.stringValue = ToNSString(AppSupport::isoDateToday());
    _expenseNoteField.stringValue = @"";
    _selectionHintLabel.stringValue = @"Select a row below if you want to update or delete it.";
    [_tableView deselectAll:nil];
}

- (void)refreshUI {
    const auto [currentYear, currentMonth] = AppSupport::currentYearMonth();
    const auto loadedState = _service->getState();
    _service->initializeIfNeeded(currentYear, currentMonth, loadedState.getYear() == 0 ? 5200 : loadedState.getSalary());
    [self checkGoalNotifications];

    const auto state = _service->getState();
    [self reloadMonthPickerKeepingSelection];
    const auto [selectedYear, selectedMonth] = [self selectedMonth];

    const auto metrics = AppSupport::calculateMetrics(*_service, selectedYear, selectedMonth);
    const auto chart = AppSupport::buildLastMonthsChart(*_service, 6, selectedYear, selectedMonth);
    const auto ideas = AppSupport::buildSavingIdeas(*_service, selectedYear, selectedMonth);

    const BOOL current = (selectedYear == currentYear && selectedMonth == currentMonth);
    const double leftForMonth = current
        ? state.getAvailable()
        : state.getSalary() - state.getTotalFixedBills() - state.getMonthlySavingsContribution() - metrics.spentThisMonth;

    _salaryField.stringValue = [NSString stringWithFormat:@"%.0f", state.getSalary()];
    _monthlySavingField.stringValue = [NSString stringWithFormat:@"%.0f", state.getMonthlySavingsContribution()];
    if (_expenseDateField.stringValue.length == 0) {
        _expenseDateField.stringValue = ToNSString(AppSupport::isoDateToday());
    }

    _moneyLeftLabel.stringValue = [NSString stringWithFormat:@"%.0f lei", leftForMonth];
    _savedLabel.stringValue = [NSString stringWithFormat:@"%.0f lei", state.getSavings()];
    _spentMonthLabel.stringValue = [NSString stringWithFormat:@"%.0f lei", metrics.spentThisMonth];
    _billsLabel.stringValue = [NSString stringWithFormat:@"%.0f lei", metrics.fixedBillsTotal];
    _perDayLabel.stringValue = [NSString stringWithFormat:@"%.0f lei", metrics.safeToSpendPerDay];

    [self rebuildFixedBills:state];
    [self rebuildGoals:state];
    [self rebuildIdeas:ideas average:metrics.averageMonthlySpending];
    [_chartView setPoints:chart selectedIndex:static_cast<NSInteger>(chart.size() - 1)];

    _monthExpenses = AppSupport::expensesForMonth(*_service, selectedYear, selectedMonth);
    std::sort(_monthExpenses.begin(), _monthExpenses.end(), [](const Expense& left, const Expense& right) {
        if (left.getDate() == right.getDate()) {
            return left.getId() > right.getId();
        }
        return left.getDate() > right.getDate();
    });
    [_tableView reloadData];
}

- (IBAction)changeMonth:(id)sender {
    (void)sender;
    [self clearExpenseForm];
    [self refreshUI];
}

- (IBAction)saveSalary:(id)sender {
    (void)sender;
    try {
        _service->setMonthlySalary(std::stod(ToStdString(_salaryField.stringValue)));
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)saveAutoSaving:(id)sender {
    (void)sender;
    try {
        _service->setMonthlySavingsContribution(std::stod(ToStdString(_monthlySavingField.stringValue)));
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)addFixedBill:(id)sender {
    (void)sender;
    try {
        _service->addFixedBill(ToStdString(_fixedBillNameField.stringValue), std::stod(ToStdString(_fixedBillAmountField.stringValue)));
        _fixedBillNameField.stringValue = @"";
        _fixedBillAmountField.stringValue = @"";
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)removeFixedBill:(id)sender {
    (void)sender;
    try {
        _service->removeFixedBill(ToStdString(_fixedBillNameField.stringValue));
        _fixedBillNameField.stringValue = @"";
        _fixedBillAmountField.stringValue = @"";
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)addGoal:(id)sender {
    (void)sender;
    try {
        _service->addGoal(ToStdString(_goalNameField.stringValue), std::stod(ToStdString(_goalAmountField.stringValue)));
        _goalNameField.stringValue = @"";
        _goalAmountField.stringValue = @"";
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)removeGoal:(id)sender {
    (void)sender;
    try {
        _service->removeGoal(ToStdString(_goalPicker.titleOfSelectedItem));
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)estimateSelectedGoal:(id)sender {
    (void)sender;
    try {
        const auto state = _service->getState();
        const std::string selectedName = ToStdString(_goalPicker.titleOfSelectedItem);
        auto it = std::find_if(state.getGoals().begin(), state.getGoals().end(),
                               [&](const SavingsGoal& goal) { return goal.name == selectedName; });
        if (it == state.getGoals().end()) {
            throw std::runtime_error("Choose a saved goal first.");
        }
        const int months = _service->monthsToAfford(it->target);
        if (months <= 0) {
            _goalEstimateField.stringValue = months == 0
                ? @"You can already afford this goal now."
                : @"With the current plan, this goal is still too hard to reach.";
        } else {
            _goalEstimateField.stringValue = [NSString stringWithFormat:@"For \"%@\" you may need about %d month(s).", ToNSString(it->name), months];
        }
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)quickSave:(id)sender {
    (void)sender;
    try {
        _service->transferToSavings(std::stod(ToStdString(_quickSaveField.stringValue)));
        _quickSaveField.stringValue = @"";
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)addExpense:(id)sender {
    (void)sender;
    try {
        const std::string date = ToStdString(_expenseDateField.stringValue).empty()
            ? AppSupport::isoDateToday()
            : ToStdString(_expenseDateField.stringValue);
        _service->addExpense(AppSupport::nextExpenseId(*_service),
                             std::stod(ToStdString(_expenseAmountField.stringValue)),
                             ToStdString(_expenseCategoryPopup.titleOfSelectedItem),
                             date,
                             ToStdString(_expenseNoteField.stringValue));
        [self clearExpenseForm];
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)updateExpense:(id)sender {
    (void)sender;
    try {
        if (!_selectedExpenseId.has_value()) {
            throw std::runtime_error("Select an expense row first.");
        }
        _service->updateExpense(*_selectedExpenseId,
                                std::stod(ToStdString(_expenseAmountField.stringValue)),
                                ToStdString(_expenseCategoryPopup.titleOfSelectedItem),
                                ToStdString(_expenseDateField.stringValue),
                                ToStdString(_expenseNoteField.stringValue));
        [self clearExpenseForm];
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (IBAction)deleteExpense:(id)sender {
    (void)sender;
    try {
        if (!_selectedExpenseId.has_value()) {
            throw std::runtime_error("Select an expense row first.");
        }
        _service->deleteExpense(*_selectedExpenseId);
        [self clearExpenseForm];
        [self refreshUI];
    } catch (const std::exception& error) {
        [self showError:error];
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    (void)tableView;
    return static_cast<NSInteger>(_monthExpenses.size());
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    (void)tableView;
    const Expense& expense = _monthExpenses[static_cast<std::size_t>(row)];
    NSString* text = @"";
    if ([tableColumn.identifier isEqualToString:@"date"]) {
        text = ToNSString(expense.getDate());
    } else if ([tableColumn.identifier isEqualToString:@"category"]) {
        text = ToNSString(expense.getCategory());
    } else if ([tableColumn.identifier isEqualToString:@"amount"]) {
        text = [NSString stringWithFormat:@"%.2f lei", expense.getAmount()];
    } else if ([tableColumn.identifier isEqualToString:@"note"]) {
        text = ToNSString(expense.getDescription());
    }
    return MakeLabel(text, 12, NO);
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {
    (void)notification;
    const NSInteger row = _tableView.selectedRow;
    if (row < 0 || row >= static_cast<NSInteger>(_monthExpenses.size())) {
        _selectedExpenseId.reset();
        _selectionHintLabel.stringValue = @"Select a row below if you want to update or delete it.";
        return;
    }

    const Expense& expense = _monthExpenses[static_cast<std::size_t>(row)];
    _selectedExpenseId = expense.getId();
    _expenseAmountField.stringValue = [NSString stringWithFormat:@"%.2f", expense.getAmount()];
    _expenseDateField.stringValue = ToNSString(expense.getDate());
    _expenseNoteField.stringValue = ToNSString(expense.getDescription());
    [_expenseCategoryPopup selectItemWithTitle:ToNSString(expense.getCategory())];
    _selectionHintLabel.stringValue = @"Selected row loaded into the form. You can update it or delete it.";
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        ExpenseAppController* delegate = [[ExpenseAppController alloc] init];
        app.delegate = delegate;
        return NSApplicationMain(argc, argv);
    }
}
