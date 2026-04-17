# WebDataExtractor

Qt/qmake workspace for building multiple site-specific web data extractors on top of a shared Qt library.

## Overview

This repository is not a single scraper. It is a collection of Qt applications that share a common extraction framework in `WDELib` plus embedded resources in `WDEResource`.

The project build target is defined in `WebDataExtractor.pro` under `SUBDIRS`.

`WDEResource` contains helpful resource.

## Design & Implementation

### Architecture Summary

In technical terms, Qt is used in this project for:

- qmake-based multi-project build orchestration
- desktop windows, layouts, dialogs, and form controls
- an embedded WebKit browser (`QWebView`) for loading pages
- DOM traversal and manipulation through `QWebElement`
- signal/slot orchestration of the scraping lifecycle
- timers, settings, URLs, containers, and file I/O
- SQLite persistence through `QtSql`
- XML export through `QtXml`
- embedded datasets and scripts through the Qt resource system

### Build System

- Target baseline: Qt `5.4.0`
- The root project `WebDataExtractor.pro` uses `TEMPLATE = subdirs`.
- Common build flags live in `WDEConfig.pri`.
- Common application settings live in `WebDataExtractor.pri`.
- Current default database mode: `USE_SQLITE`
  - Optional alternative mode exists for MySQL via `USE_DATABASE`, but it is disabled in the current configuration
- Build outputs are redirected outside the repo:
  - debug: `../wde-build-debug`
  - release: `../wde-build`


### Qt Modules Enabled

The shared project settings enable these Qt modules:

- `QtCore`
- `QtGui`
- `QtWidgets` for Qt 5 builds
- `QtWebKit`
- `QtWebKitWidgets` for Qt 5 builds
- `QtSql`
- `QtXml`
- `QtNetwork`

Typical build flow is qmake-based, for example:

```bash
qmake WebDataExtractor.pro
make
```

### How Qt Is Used

#### 1. Application and UI Layer

Each scraper is a standard Qt desktop application built around `QApplication` and a top-level `QDialog` or `QWidget`.

- `BaseWebEngine` in `WDELib/BaseWebEngine/basewebengine.h` is the main reusable window class
- It creates a common UI with:
  - URL input
  - embedded browser view
  - progress field
  - status field
  - Start/Stop controls
- Some projects also add a small launcher window with `QComboBox` and `QPushButton` to choose between multiple scraper targets, for example `british_horse_society_org_uk/mainwnd.cpp`

There is also a convenience macro:

```cpp
#define WebEngine_Main(WebObject) \
int main(int argc, char *argv[]) \
{ \
	QApplication app(argc, argv); \
	WebObject web; \
	web.showNormal(); \
	return app.exec(); \
}
```

That macro is used by several scraper executables to generate a minimal Qt entry point.

#### 2. Embedded Browser Layer

Important: despite names like `BaseWebEngine` and `WebEngine`, the project is using Qt WebKit APIs, not Qt WebEngine.

The actual browser implementation is in `WDELib/BaseWebEngine/webengine.*` and is based on:

- `QWebView`
- `QWebFrame`
- `QWebElement`
- `QWebElementCollection`
- `QWebSettings`

This is the core of how the scrapers interact with web pages:

- DOM queries use CSS selectors such as `findFirst(...)` and `findAll(...)`
- Links are resolved with `QUrl` against the current frame base URL
- Clicks are simulated by dispatching JavaScript mouse events onto `QWebElement`
- JavaScript can be executed directly through `QWebFrame::evaluateJavaScript(...)`
- Browser settings explicitly enable Java and JavaScript in `WebEngine`

So in practice, Qt is being used here as both:

- the desktop GUI toolkit
- the in-process browser automation layer

#### 3. Event-Driven Extraction Flow

The extraction model is signal/slot driven.

`BaseWebEngine` connects Qt WebKit and widget events such as:

- `loadStarted()`
- `loadProgress(int)`
- `loadFinished(bool)`
- `urlChanged(QUrl)`
- `statusBarMessage(QString)`
- button click signals for Start/Stop

Each concrete scraper subclasses `BaseWebEngine` and overrides:

- `doStart()`
- `doStop()`
- `doWebLoadFinished()`
- `initDatabase()`
- `initExtWidget()`

The usual scraper lifecycle is:

1. Start from a category or search page
2. Extract detail URLs from the DOM
3. Store those URLs in memory or a temporary SQLite table
4. Visit each detail page through the embedded `QWebView`
5. Read fields from the DOM with `QWebElement`
6. Save the structured result

State transitions are tracked with the `ENU_STATE` enum in `BaseWebEngine`.

#### 4. Timers, Timeouts, and Retry Behavior

Qt timers are used heavily to keep the scraper flow event-driven instead of blocking the UI thread with manual sleeps.

- `QTimer` is used for page-load timeout control
- a pause timer is used to resume after failed loads
- another timer is used to add randomized delays between actions

The default web timeout constant is:

- `DEF_INT_TIMER_STOP_LOAD = 40000`

This gives each page load a 40 second timeout unless overridden from the UI extension.

#### 5. Shared Extraction Controls

Most scrapers attach `ExtFileLoading` from `WDELib/Extension/extfileloading.*` as an extra Qt widget under the main browser window.

That shared Qt widget provides:

- `Extract link only` mode
- input file browsing with `QFileDialog`
- skip-item count
- custom web timeout
- extra category/page parameters

This is a reusable Qt control panel for switching between:

- collecting URLs only
- replaying previously collected URLs from file or temp storage

### Data Handling and Persistence

#### Data Model

`DataEntry` is the shared result container.

- internally it stores data as `QMap<QString, QStringList>`
- single-value and multi-value fields are both supported
- scraped text is normalized before saving
- entries can be converted into SQL insert statements

#### SQL Generation

`SQLQueryBuilder` generates the SQL used by the project.

With `USE_SQLITE` enabled, it switches behavior to SQLite-friendly statements such as:

- `CREATE TABLE IF NOT EXISTS ...`
- `INSERT OR IGNORE ...`

#### Logging Formats

Qt I/O classes are also used for export/logging:

- `Logger` writes plain text logs using `QFile` and `QTextStream`
- `DBLogger` writes SQL scripts
- `XMLLogger` writes appendable XML using `QXmlStreamWriter`

So the project uses Qt not only for scraping pages, but also for the full data pipeline around those pages.
