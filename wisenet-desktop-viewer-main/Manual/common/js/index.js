/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = 0);
/******/ })
/************************************************************************/
/******/ ({

/***/ "./src/scripts/index.ts":
/*!******************************!*\
  !*** ./src/scripts/index.ts ***!
  \******************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

exports.__esModule = true;
var helpers_1 = __webpack_require__(/*! ./modules/helpers */ "./src/scripts/modules/helpers.ts");
var content = __webpack_require__(/*! ./modules/content */ "./src/scripts/modules/content.ts");
var searchResults = __webpack_require__(/*! ./modules/searchResults */ "./src/scripts/modules/searchResults.ts");
var search_1 = __webpack_require__(/*! ./modules/search */ "./src/scripts/modules/search.ts");
var summary = __webpack_require__(/*! ./modules/summary */ "./src/scripts/modules/summary.ts");
// MAIN
$(function () {
    $(helpers_1.contentItem()).show();
    // add listener for the content selection
    window.onhashchange = function () {
        content.show(helpers_1.getSelectedContentId());
    };
    // if there is a content selected in a link - react to it
    // because it can not be handled in onhashchange handler
    content.show(helpers_1.getSelectedContentId());
    
    
   
    $(document).on('click', function (e) {
        var $target = $(e.target);
        /*
        if ($target.is('[data-search-result]') || $target.parents('[data-search-result]').length > 0) {
            var id = searchResults.clickHandler($target);
            helpers_1.setContentUrl(id);
            content.show(id);
            // add scroll to found item
            var resultID = $target.find('[data-result-id]').attr('data-result-id');
            var offset = $(helpers_1.contentItem(id)).find("[data-result-id=\"" + resultID + "\"]").offset().top;
            $('.content').scrollTop(offset > 300 ? offset : 0);
        }
        */
        
        if ($target.parents('[data-search-result]').length === 0 && !$target.is('[data-search]')) {
            //searchResults.remove();
        }
        /*
        if ($target.is(helpers_1.sectionItem())) {
            e.preventDefault();
            summary.itemClickHandler($target.attr('data-section-item'));
            $('.content').scrollTop(0);
        }
        */
        if ($target.is('[data-print]')) {
            window.print();
        }
        if ($target.is('[data-search]')) {
            searchBtnHandler($target);
        }
        if ($target.is('[data-reset]')) {
            resetBtnHandler($target);
        }
        if ($target.closest('[data-menu-btn').length > 0) {
            $('[data-summary]').toggleClass('wrapper__summary_active');
        }
    });
    
    var seinp_off = $('.ssch_close');
    
    seinp_off.click(function(){
        
        var val = $(this).val();
        searchResults.remove();
        removePreviousHighlight();
        validateSearchInput(val);

        
        //$('.content__search-result').removeClass('content__search-result');
        
    }); 
   

            

    
    
    $('[data-search], [data-print]').on("mousedown mouseup", function (e) {
        // @ts-ignore
        if (e.handleObj.type === "mouseup") {
            $(this).removeClass('pressed');
            // @ts-ignore
        }
        else if (e.handleObj.type === "mousedown") {
            $(this).addClass('pressed');
        }
    });
    $('[data-search-field]').on('keyup input', function (e) {
        var val = $(this).val();
        searchResults.remove();
        removePreviousHighlight();
        validateSearchInput(val);
        if (e.which == 13) {
            $('[data-search]').trigger('click');
        }
        else if (e.which == 27) {
            $('[data-reset]').trigger('click');
        }
    });
    //createSmallImages();
});
function resetBtnHandler($target) {
    searchResults.remove();
    removePreviousHighlight();
    $('[data-search-field]').val('');
    validateSearchInput($('[data-search-field]').val());
}
function searchBtnHandler($target) {
    // if inactive - do nothing
    if ($target.attr('data-is-active') === 'false')
        return;
    var val = $('[data-search-field]').val().toString();
    if (val) {
        var safeInput = val.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
        removePreviousHighlight();
        searchResults.render(search_1["default"](safeInput.trim()));
        // force button not to react to click for one second
        $target.attr('data-is-active', 'false');
        setTimeout(function () {
            $target.attr('data-is-active', 'true');
        }, 1000);
    }
}
;
function createSmallImages() {
    $(helpers_1.contentItem() + " img").not('.content__note__img').each(function (_i, img) {
        var src = $(img).attr('src');
        if (src.indexOf('inline') !== -1 || src.indexOf('icon') !== -1) {
            $(img).addClass('content__inline-img');
        }
        else {
            $(img).addClass("content__img");
        }
    });
}
;
function validateSearchInput(value) {
    // input in general allowed for any letter from any lang, space and numbers
    // this regexp returns true for any letter or number and space or semicolon
    var wordsAndSpace = XRegExp('(\\p{L}{2,}|\\d+)\\,*\\s*\\1*');
    // this regexp returns true for any non-letter symbol except space and number or semicolon
    var specialSymbols = XRegExp("[^(\\p{L}^\\s|^\\d|^\\,|^\\-|^\\')]");
    wordsAndSpace.test(value.toString()) && !specialSymbols.test(value.toString())
        ? $('[data-search]').attr('data-is-active', 'true').addClass('header__search-btn_active')
        : $('[data-search]').attr('data-is-active', 'false').removeClass('header__search-btn_active');
    value === ''
        ? $('[data-reset]').show()
        : $('[data-reset]').show();
}
;
function removePreviousHighlight() {
    $('.content__search-result').each(function (i, elem) {
        var parent = elem.parentElement;
        $(elem).replaceWith($(elem).text());
        // after removing highlight-span text-nodes are split
        parent.normalize();
    });
}
// missing forEach on NodeList for IE11
if (window.NodeList && !NodeList.prototype.forEach) {
    // @ts-ignore
    NodeList.prototype.forEach = Array.prototype.forEach;
}
if (!Array.prototype.find) {
    Array.prototype.find = function (predicate) {
        if (this == null) {
            throw new TypeError('Array.prototype.find called on null or undefined');
        }
        if (typeof predicate !== 'function') {
            throw new TypeError('predicate must be a function');
        }
        var list = Object(this);
        var length = list.length >>> 0;
        var thisArg = arguments[1];
        var value;
        for (var i = 0; i < length; i++) {
            value = list[i];
            if (predicate.call(thisArg, value, i, list)) {
                return value;
            }
        }
        return undefined;
    };
}


/***/ }),

/***/ "./src/scripts/modules/content.ts":
/*!****************************************!*\
  !*** ./src/scripts/modules/content.ts ***!
  \****************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

exports.__esModule = true;
var helpers_1 = __webpack_require__(/*! ./helpers */ "./src/scripts/modules/helpers.ts");
// function is fired every time when hash in url changes
function show(id) {
    $(helpers_1.contentItem()).show();
    var $activeContent = $(helpers_1.contentItem(id));
    var $activeSummaryItem = $(helpers_1.sectionItem(id));
    var OPEN_CLASS = "opened";
    var NO_BORDER_CLASS = "content__section_no-border";
    if ($activeContent.is('[data-content-level="3"]')) {
        // content of level 3 is children node of the content item level 2
        // so we need to hide other level 3 items in the same content level 2
        $activeContent.siblings().show();
        // we should display parent which is level 2,
        // because if parent node is hidden - all children are hidden also
        $activeContent.parent(helpers_1.contentItem()).show();
        // show content and remove border
        $activeContent.show().addClass(NO_BORDER_CLASS);
    }
    else {
        // add border to children level 3
        $activeContent.children("." + NO_BORDER_CLASS).removeClass(NO_BORDER_CLASS);
        // if it is a level two content - it can have a level 3 sections,
        // which are we should display also
        $activeContent.show().children().show();
    }
    // we remove all active classes from summary items
    $(helpers_1.section()).not($activeSummaryItem.parents()).removeClass(OPEN_CLASS);
    // toggle opened and active class of section elem
    $activeSummaryItem.parents(helpers_1.section()).addClass(OPEN_CLASS);
}
exports.show = show;


/***/ }),

/***/ "./src/scripts/modules/helpers.ts":
/*!****************************************!*\
  !*** ./src/scripts/modules/helpers.ts ***!
  \****************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

exports.__esModule = true;
/** Return data-section selector for elements from summary */
function section(level) {
    if (level === void 0) { level = null; }
    return level
        ? "[data-section=\"" + level + "\"]"
        : '[data-section]';
}
exports.section = section;
/** Returns data-section-item selector for summary items (headings) */
function sectionItem(id) {
    if (id === void 0) { id = ""; }
    return id === ""
        ? '[data-section-item]'
        : "[data-section-item=\"" + id + "\"";
}
exports.sectionItem = sectionItem;
/** Returns data-content selector for parts of content under given id */
function contentItem(id) {
    if (id === void 0) { id = ""; }
    return id === ""
        ? '[data-content]'
        : "[data-content=\"" + id + "\"]";
}
exports.contentItem = contentItem;
/** Sets window hash to emulate default anchor links behavior */
function setContentUrl(id) {
    window.location.hash = "#" + id;
    return window.location.href;
}
exports.setContentUrl = setContentUrl;
/** Returns selected content or first selectable */
function getSelectedContentId() {
    return window.location.hash === ""
        ? $(section(2)).first().children(sectionItem()).first().attr('data-section-item')
        : window.location.hash.replace("#", "");
}
exports.getSelectedContentId = getSelectedContentId;


/***/ }),

/***/ "./src/scripts/modules/search.ts":
/*!***************************************!*\
  !*** ./src/scripts/modules/search.ts ***!
  \***************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

exports.__esModule = true;
var helpers_1 = __webpack_require__(/*! ./helpers */ "./src/scripts/modules/helpers.ts");
function search(input) {
    var results = [];
    // check content sections for a matches
    $(helpers_1.contentItem()).each(function (_i, elem) {
        elem.normalize();
        var regexp = new RegExp(input + "(?!_|!-|!.)", 'ig');
        var matches = $(elem).text().match(regexp);
        if (matches) {
            var resultsArr = [];
            searchLastElem(elem, regexp, resultsArr);
            if (resultsArr.length > 0) {
                resultsArr.forEach(function (result) {
                    var $elem = result[0];
                    var id = $elem.closest('[data-content]').attr('data-content');
                    var html = $elem.html(); //createHighlightedHtml(result[0], result[1]);
                    //if content of note
                    if ($elem.parents('div.content__note').length > 0) {
                        $elem = $elem.parents('div.content__note');
                        html = $elem.get(0).outerHTML;
                    }
                    results.push(createResult(id, html));
                });
            }
        }
    });
    return removeDuplicates(results);
}
exports["default"] = search;
function removeDuplicates(results) {
    // here we will remove all results with equal text and/or equal amout of results highlighted
    results.forEach(function (elem, i) {
        var $current = $("<div>" + (elem.html + elem.contentID) + "</div>");
        var duplicate = results.find(function (item, j) {
            var isTextEqual = $("<div>" + (item.html + item.contentID) + "</div>").text() === $current.text();
            var isItemIncludeCurrentText = $("<div>" + (item.html + item.contentID) + "</div>").text().indexOf($current.text()) !== -1;
            return (isTextEqual || isItemIncludeCurrentText)
                && i !== j;
        });
        if (duplicate) {
            var amountOfResults = $current.find('.content__search-result').length;
            var amountOfResultInDuplicate = $("<div>" + duplicate.html + "</div>").find('.content__search-result').length;
            var level = $(helpers_1.sectionItem(elem.contentID)).parent().attr('data-section');
            var levelOfDuplicate = $(helpers_1.sectionItem(duplicate.contentID)).parent().attr('data-section');
            if (amountOfResults <= amountOfResultInDuplicate && level >= levelOfDuplicate) {
                results.splice(i, 1);
            }
            ;
        }
    });
    return results;
}
// recursively search last text node that contains search input
// and writes all nodes to results array
function searchLastElem(elem, regexp, resultsArray) {
    var children = elem.childNodes;
    var result;
    children.forEach(function (child, i) {
        // @ts-ignore
        if (child.parentNode.className === 'content__search-result')
            return;
        if (child.nodeName !== "#text") {
            searchLastElem(child, regexp, resultsArray);
        }
        else {
            var text = child.textContent;
            var match = text.match(regexp);
            if (match) {
                var $parent = $(child.parentNode);
                // here we take a parent of text node, not a text only
                // we will need it later for highlighting result on a page 
                //highliting
                child.nodeValue = text.replace(regexp, function (match, offset, input) {
                    // колхозный способ проверить не обернут ли матч в спан с классом
                    // content__search-result
                    if (input[offset - 1] === '>' && input[offset - 3] === 't') {
                        return match;
                    }
                    else {
                        return "<span class=\"content__search-result\" data-result-id=\"" + Math.floor(Math.random() * 1000) + "\">" + match + "</span>";
                    }
                });
                $parent.html(replaceTagEscapeChar($parent.html()));
                if ($parent.is('b') || $parent.is('strong') || $parent.is('a')) {
                    $parent = $parent.parent();
                }
                ;
                result = [$parent];
            }
        }
    });
    if (result) {
        resultsArray.push(result);
    }
}
function replaceTagEscapeChar(escapedChar) {
    var result = escapedChar;
    result = result.replace(/\&gt\;/g, '>');
    result = result.replace(/\&lt\;/g, '<');
    return result;
}
function createHighlightedHtml($elem, matches) {
    var resultHtml;
    var src = $elem.find('img').attr('src');
    matches.forEach(function (match) {
        var regexp = new RegExp(match, 'ig');
        $elem.find('img').attr('src', '');
        resultHtml = $elem.html().replace(regexp, function (match, offset, input) {
            // колхозный способ проверить не обернут ли матч в спан с классом
            // content__search-result
            if (input[offset - 1] === '>' && input[offset - 3] === 't') {
                return match;
            }
            else {
                return "<span class=\"content__search-result\" data-result-id=\"" + Math.floor(Math.random() * 1000) + "\">" + match + "</span>";
            }
        });
        $elem.html(resultHtml);
    });
    $elem.find('img').attr('src', src);
    if ($elem.parents('div.content__note').length > 0) {
        $elem = $elem.parents('div.content__note');
        resultHtml = $elem.get(0).outerHTML;
    }
    else {
        resultHtml = $elem.html();
    }
    return resultHtml;
}
;
function createResult(id, html) {
    return {
        contentID: id,
        html: html
    };
}


/***/ }),

/***/ "./src/scripts/modules/searchResults.ts":
/*!**********************************************!*\
  !*** ./src/scripts/modules/searchResults.ts ***!
  \**********************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

exports.__esModule = true;
var helpers_1 = __webpack_require__(/*! ./helpers */ "./src/scripts/modules/helpers.ts");
/** Render search result to a dropdown */
function render(results) {
    remove();
    if (results.length === 0) {
        var msg = $('[data-nothing-found-msg]').attr('data-nothing-found-msg');
        $("<div class=\"header__search__results__item\"><span class=\"search-item__preview\">" + msg + "</span></div>")
            .appendTo('[data-search-results]');

    }
    else {
        results.forEach(function (result) {
            _renderItem(result);
            $(".search-item a").remove();
            $(".search-item h4").remove();

        });
    }
    $('[data-search-results]').show('fast').scrollTop(0);
    
            ////////////////////페이징
            $('.search-item').click(function(){
                var datas = $(this).data('search-result');
                
                document.querySelector('.p' + datas).click();
                //$('.item_div').hide();
                //$('.' + datas).show();
                //location.href='#'+datas;
                //$('.pp0302').trigger('click');
                //console.log(datas);
                
            });
    
}
exports.render = render;
;
/** Returns id to show */
function clickHandler($target) {
    $('[data-search-results]').hide('fast');
    return $target.closest('[data-search-result]').attr('data-search-result');
}
exports.clickHandler = clickHandler;
function _renderItem(result) {
    // array of parents ids in summary for breadcrumbs
    var paths = [];
    // summary item
    var $item = $("[data-section-item=\"" + result.contentID + "\"]");
    // create a breadcrumbs
    
    $item.parents(helpers_1.section()).siblings(helpers_1.sectionItem()).each(function (i, elem) {
        paths.push($(elem).text());
    });
    // last item in breadcrumbs - our summary item text
    paths.push($item.text());
    $(itemTemplate(result.contentID, paths.join(" > "), result.html)).appendTo('[data-search-results]');
    function itemTemplate(id, pathString, html) {
        return "\n      <div class=\"header__search__results__item search-item\" data-search-result=\"" + id + "\">\n        <span class=\"search-item__path\">" + pathString + "</span>\n        <p class=\"search-item__preview\">" + html + "</p>\n      </div>";
        $('[data-search-results]').show('fast').scrollTop(0);
    }

    $('.search-item__preview').children('.viewbtns_s').removeAttr('data-id');
    $('.search-item__preview').children('.viewbtns_s').removeAttr('onclick');
    $('.search-item__preview').children('.viewbtns_s').removeAttr('class');
}
;
/** Remove all children from a search results dropdown */
function remove() {
    $('[data-search-results]').show().children().remove();
}
exports.remove = remove;


/***/ }),

/***/ "./src/scripts/modules/summary.ts":
/*!****************************************!*\
  !*** ./src/scripts/modules/summary.ts ***!
  \****************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

exports.__esModule = true;
var helpers_1 = __webpack_require__(/*! ./helpers */ "./src/scripts/modules/helpers.ts");
/** Handles click on summary item, and checks is there a content under it */
function itemClickHandler(id) {
    // if clicked item is an level 2 item - it can has no content
    if ($(helpers_1.contentItem(id)).length === 0) {
        // so we should select first inner level 3 item with content
        // if any inner level 3 item selected - do nothing, we ok 
        if ($(helpers_1.sectionItem(id)).parent().hasClass('opened'))
            return;
        // else - we find first inner item with content and select it 
        var newId = $(helpers_1.sectionItem(id)).siblings(helpers_1.section()).first().children(helpers_1.sectionItem()).first().attr('data-section-item');
        helpers_1.setContentUrl(newId);
    }
    else {
        helpers_1.setContentUrl(id);
    }
}
exports.itemClickHandler = itemClickHandler;


/***/ }),

/***/ 0:
/*!************************************!*\
  !*** multi ./src/scripts/index.ts ***!
  \************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

module.exports = __webpack_require__(/*! C:\Users\MindAll\Desktop\Projects\로그러스IT\20200204\WISENET_source\src\scripts\index.ts */"./src/scripts/index.ts");


/***/ })

/******/ });
//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly8vd2VicGFjay9ib290c3RyYXAiLCJ3ZWJwYWNrOi8vLy4vc3JjL3NjcmlwdHMvaW5kZXgudHMiLCJ3ZWJwYWNrOi8vLy4vc3JjL3NjcmlwdHMvbW9kdWxlcy9jb250ZW50LnRzIiwid2VicGFjazovLy8uL3NyYy9zY3JpcHRzL21vZHVsZXMvaGVscGVycy50cyIsIndlYnBhY2s6Ly8vLi9zcmMvc2NyaXB0cy9tb2R1bGVzL3NlYXJjaC50cyIsIndlYnBhY2s6Ly8vLi9zcmMvc2NyaXB0cy9tb2R1bGVzL3NlYXJjaFJlc3VsdHMudHMiLCJ3ZWJwYWNrOi8vLy4vc3JjL3NjcmlwdHMvbW9kdWxlcy9zdW1tYXJ5LnRzIl0sIm5hbWVzIjpbXSwibWFwcGluZ3MiOiI7UUFBQTtRQUNBOztRQUVBO1FBQ0E7O1FBRUE7UUFDQTtRQUNBO1FBQ0E7UUFDQTtRQUNBO1FBQ0E7UUFDQTtRQUNBO1FBQ0E7O1FBRUE7UUFDQTs7UUFFQTtRQUNBOztRQUVBO1FBQ0E7UUFDQTs7O1FBR0E7UUFDQTs7UUFFQTtRQUNBOztRQUVBO1FBQ0E7UUFDQTtRQUNBLDBDQUEwQyxnQ0FBZ0M7UUFDMUU7UUFDQTs7UUFFQTtRQUNBO1FBQ0E7UUFDQSx3REFBd0Qsa0JBQWtCO1FBQzFFO1FBQ0EsaURBQWlELGNBQWM7UUFDL0Q7O1FBRUE7UUFDQTtRQUNBO1FBQ0E7UUFDQTtRQUNBO1FBQ0E7UUFDQTtRQUNBO1FBQ0E7UUFDQTtRQUNBLHlDQUF5QyxpQ0FBaUM7UUFDMUUsZ0hBQWdILG1CQUFtQixFQUFFO1FBQ3JJO1FBQ0E7O1FBRUE7UUFDQTtRQUNBO1FBQ0EsMkJBQTJCLDBCQUEwQixFQUFFO1FBQ3ZELGlDQUFpQyxlQUFlO1FBQ2hEO1FBQ0E7UUFDQTs7UUFFQTtRQUNBLHNEQUFzRCwrREFBK0Q7O1FBRXJIO1FBQ0E7OztRQUdBO1FBQ0E7Ozs7Ozs7Ozs7Ozs7QUNsRmE7QUFDYjtBQUNBLGdCQUFnQixtQkFBTyxDQUFDLDJEQUFtQjtBQUMzQyxjQUFjLG1CQUFPLENB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O0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQSxpQkFBaUI7QUFDakI7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBLEtBQUs7QUFDTDtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQSxtQ0FBbUM7QUFDbkMsbUNBQW1DO0FBQ25DO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQSxTQUFTO0FBQ1Q7QUFDQSxLQUFLO0FBQ0w7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTs7Ozs7Ozs7Ozs7OztBQ3pJYTtBQUNiO0FBQ0EsZ0JBQWdCLG1CQUFPLENBQUMsbURBQVc7QUFDbkM7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBLFNBQVM7QUFDVDtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBLEtBQUs7QUFDTDtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTs7Ozs7Ozs7Ozs7OztBQy9DYTtBQUNiO0FBQ0EsZ0JBQWdCLG1CQUFPLENBQUMsbURBQVc7QUFDbkM7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQSIsImZpbGUiOiJpbmRleC5qcyIsInNvdXJjZXNDb250ZW50IjpbIiBcdC8vIFRoZSBtb2R1bGUgY2FjaGVcbiBcdHZhciBpbnN0YWxsZWRNb2R1bGVzID0ge307XG5cbiBcdC8vIFRoZSByZXF1aXJlIGZ1bmN0aW9uXG4gXHRmdW5jdGlvbiBfX3dlYnBhY2tfcmVxdWlyZV9fKG1vZHVsZUlkKSB7XG5cbiBcdFx0Ly8gQ2hlY2sgaWYgbW9kdWxlIGlzIGluIGNhY2hlXG4gXHRcdGlmKGluc3RhbGxlZE1vZHVsZXNbbW9kdWxlSWRdKSB7XG4gXHRcdFx0cmV0dXJuIGluc3RhbGxlZE1vZHVsZXNbbW9kdWxlSWRdLmV4cG9ydHM7XG4gXHRcdH1cbiBcdFx0Ly8gQ3JlYXRlIGEgbmV3IG1vZHVsZSAoYW5kIHB1dCBpdCBpbnRvIHRoZSBjYWNoZSlcbiBcdFx0dmFyIG1vZHVsZSA9IGluc3RhbGxlZE1vZHVsZXNbbW9kdWxlSWRdID0ge1xuIFx0XHRcdGk6IG1vZHVsZUlkLFxuIFx0XHRcdGw6IGZhbHNlLFxuIFx0XHRcdGV4cG9ydHM6IHt9XG4gXHRcdH07XG5cbiBcdFx0Ly8gRXhlY3V0ZSB0aGUgbW9kdWxlIGZ1bmN0aW9uXG4gXHRcdG1vZHVsZXNbbW9kdWxlSWRdLmNhbGwobW9kdWxlLmV4cG9ydHMsIG1vZHVsZSwgbW9kdWxlLmV4cG9ydHMsIF9fd2VicGFja19yZXF1aXJlX18pO1xuXG4gXHRcdC8vIEZsYWcgdGhlIG1vZHVsZSBhcyBsb2FkZWRcbiBcdFx0bW9kdWxlLmwgPSB0cnVlO1xuXG4gXHRcdC8vIFJldHVybiB0aGUgZXhwb3J0cyBvZiB0aGUgbW9kdWxlXG4gXHRcdHJldHVybiBtb2R1bGUuZXhwb3J0cztcbiBcdH1cblxuXG4gXHQvLyBleHBvc2UgdGhlIG1vZHVsZXMgb2JqZWN0IChfX3dlYnBhY2tfbW9kdWxlc19fKVxuIFx0X193ZWJwYWNrX3JlcXVpcmVfXy5tID0gbW9kdWxlcztcblxuIFx0Ly8gZXhwb3NlIHRoZSBtb2R1bGUgY2FjaGVcbiBcdF9fd2VicGFja19yZXF1aXJlX18uYyA9IGluc3RhbGxlZE1vZHVsZXM7XG5cbiBcdC8vIGRlZmluZSBnZXR0ZXIgZnVuY3Rpb24gZm9yIGhhcm1vbnkgZXhwb3J0c1xuIFx0X193ZWJwYWNrX3JlcXVpcmVfXy5kID0gZnVuY3Rpb24oZXhwb3J0cywgbmFtZSwgZ2V0dGVyKSB7XG4gXHRcdGlmKCFfX3dlYnBhY2tfcmVxdWlyZV9fLm8oZXhwb3J0cywgbmFtZSkpIHtcbiBcdFx0XHRPYmplY3QuZGVmaW5lUHJvcGVydHkoZXhwb3J0cywgbmFtZSwgeyBlbnVtZXJhYmxlOiB0cnVlLCBnZXQ6IGdldHRlciB9KTtcbiBcdFx0fVxuIFx0fTtcblxuIFx0Ly8gZGVmaW5lIF9fZXNNb2R1bGUgb24gZXhwb3J0c1xuIFx0X193ZWJwYWNrX3JlcXVpcmVfXy5yID0gZnVuY3Rpb24oZXhwb3J0cykge1xuIFx0XHRpZih0eXBlb2YgU3ltYm9sICE9PSAndW5kZWZpbmVkJyAmJiBTeW1ib2wudG9TdHJpbmdUYWcpIHtcbiBcdFx0XHRPYmplY3QuZGVmaW5lUHJvcGVydHkoZXhwb3J0cywgU3ltYm9sLnRvU3RyaW5nVGFnLCB7IHZhbHVlOiAnTW9kdWxlJyB9KTtcbiBcdFx0fVxuIFx0XHRPYmplY3QuZGVmaW5lUHJvcGVydHkoZXhwb3J0cywgJ19fZXNNb2R1bGUnLCB7IHZhbHVlOiB0cnVlIH0pO1xuIFx0fTtcblxuIFx0Ly8gY3JlYXRlIGEgZmFrZSBuYW1lc3BhY2Ugb2JqZWN0XG4gXHQvLyBtb2RlICYgMTogdmFsdWUgaXMgYSBtb2R1bGUgaWQsIHJlcXVpcmUgaXRcbiBcdC8vIG1vZGUgJiAyOiBtZXJnZSBhbGwgcHJvcGVydGllcyBvZiB2YWx1ZSBpbnRvIHRoZSBuc1xuIFx0Ly8gbW9kZSAmIDQ6IHJldHVybiB2YWx1ZSB3aGVuIGFscmVhZHkgbnMgb2JqZWN0XG4gXHQvLyBtb2RlICYgOHwxOiBiZWhhdmUgbGlrZSByZXF1aXJlXG4gXHRfX3dlYnBhY2tfcmVxdWlyZV9fLnQgPSBmdW5jdGlvbih2YWx1ZSwgbW9kZSkge1xuIFx0XHRpZihtb2RlICYgMSkgdmFsdWUgPSBfX3dlYnBhY2tfcmVxdWlyZV9fKHZhbHVlKTtcbiBcdFx0aWYobW9kZSAmIDgpIHJldHVybiB2YWx1ZTtcbiBcdFx0aWYoKG1vZGUgJiA0KSAmJiB0eXBlb2YgdmFsdWUgPT09ICdvYmplY3QnICYmIHZhbHVlICYmIHZhbHVlLl9fZXNNb2R1bGUpIHJldHVybiB2YWx1ZTtcbiBcdFx0dmFyIG5zID0gT2JqZWN0LmNyZWF0ZShudWxsKTtcbiBcdFx0X193ZWJwYWNrX3JlcXVpcmVfXy5yKG5zKTtcbiBcdFx0T2JqZWN0LmRlZmluZVByb3BlcnR5KG5zLCAnZGVmYXVsdCcsIHsgZW51bWVyYWJsZTogdHJ1ZSwgdmFsdWU6IHZhbHVlIH0pO1xuIFx0XHRpZihtb2RlICYgMiAmJiB0eXBlb2YgdmFsdWUgIT0gJ3N0cmluZycpIGZvcih2YXIga2V5IGluIHZhbHVlKSBfX3dlYnBhY2tfcmVxdWlyZV9fLmQobnMsIGtleSwgZnVuY3Rpb24oa2V5KSB7IHJldHVybiB2YWx1ZVtrZXldOyB9LmJpbmQobnVsbCwga2V5KSk7XG4gXHRcdHJldHVybiBucztcbiBcdH07XG5cbiBcdC8vIGdldERlZmF1bHRFeHBvcnQgZnVuY3Rpb24gZm9yIGNvbXBhdGliaWxpdHkgd2l0aCBub24taGFybW9ueSBtb2R1bGVzXG4gXHRfX3dlYnBhY2tfcmVxdWlyZV9fLm4gPSBmdW5jdGlvbihtb2R1bGUpIHtcbiBcdFx0dmFyIGdldHRlciA9IG1vZHVsZSAmJiBtb2R1bGUuX19lc01vZHVsZSA/XG4gXHRcdFx0ZnVuY3Rpb24gZ2V0RGVmYXVsdCgpIHsgcmV0dXJuIG1vZHVsZVsnZGVmYXVsdCddOyB9IDpcbiBcdFx0XHRmdW5jdGlvbiBnZXRNb2R1bGVFeHBvcnRzKCkgeyByZXR1cm4gbW9kdWxlOyB9O1xuIFx0XHRfX3dlYnBhY2tfcmVxdWlyZV9fLmQoZ2V0dGVyLCAnYScsIGdldHRlcik7XG4gXHRcdHJldHVybiBnZXR0ZXI7XG4gXHR9O1xuXG4gXHQvLyBPYmplY3QucHJvdG90eXBlLmhhc093blByb3BlcnR5LmNhbGxcbiBcdF9fd2VicGFja19yZXF1aXJlX18ubyA9IGZ1bmN0aW9uKG9iamVjdCwgcHJvcGVydHkpIHsgcmV0dXJuIE9iamVjdC5wcm90b3R5cGUuaGFzT3duUHJvcGVydHkuY2FsbChvYmplY3QsIHByb3BlcnR5KTsgfTtcblxuIFx0Ly8gX193ZWJwYWNrX3B1YmxpY19wYXRoX19cbiBcdF9fd2VicGFja19yZXF1aXJlX18ucCA9IFwiXCI7XG5cblxuIFx0Ly8gTG9hZCBlbnRyeSBtb2R1bGUgYW5kIHJldHVybiBleHBvcnRzXG4gXHRyZXR1cm4gX193ZWJwYWNrX3JlcXVpcmVfXyhfX3dlYnBhY2tfcmVxdWlyZV9fLnMgPSAwKTtcbiIsIlwidXNlIHN0cmljdFwiO1xyXG5leHBvcnRzLl9fZXNNb2R1bGUgPSB0cnVlO1xyXG52YXIgaGVscGVyc18xID0gcmVxdWlyZShcIi4vbW9kdWxlcy9oZWxwZXJzXCIpO1xyXG52YXIgY29udGVudCA9IHJlcXVpcmUoXCIuL21vZHVsZXMvY29udGVudFwiKTtcclxudmFyIHNlYXJjaFJlc3VsdHMgPSByZXF1aXJlKFwiLi9tb2R1bGVzL3NlYXJjaFJlc3VsdHNcIik7XHJcbnZhciBzZWFyY2hfMSA9IHJlcXVpcmUoXCIuL21vZHVsZXMvc2VhcmNoXCIpO1xyXG52YXIgc3VtbWFyeSA9IHJlcXVpcmUoXCIuL21vZHVsZXMvc3VtbWFyeVwiKTtcclxuLy8gTUFJTlxyXG4kKGZ1bmN0aW9uICgpIHtcclxuICAgICQoaGVscGVyc18xLmNvbnRlbnRJdGVtKCkpLmhpZGUoKTtcclxuICAgIC8vIGFkZCBsaXN0ZW5lciBmb3IgdGhlIGNvbnRlbnQgc2VsZWN0aW9uXHJcbiAgICB3aW5kb3cub25oYXNoY2hhbmdlID0gZnVuY3Rpb24gKCkge1xyXG4gICAgICAgIGNvbnRlbnQuc2hvdyhoZWxwZXJzXzEuZ2V0U2VsZWN0ZWRDb250ZW50SWQoKSk7XHJcbiAgICB9O1xyXG4gICAgLy8gaWYgdGhlcmUgaXMgYSBjb250ZW50IHNlbGVjdGVkIGluIGEgbGluayAtIHJlYWN0IHRvIGl0XHJcbiAgICAvLyBiZWNhdXNlIGl0IGNhbiBub3QgYmUgaGFuZGxlZCBpbiBvbmhhc2hjaGFuZ2UgaGFuZGxlclxyXG4gICAgY29udGVudC5zaG93KGhlbHBlcnNfMS5nZXRTZWxlY3RlZENvbnRlbnRJZCgpKTtcclxuICAgICQoZG9jdW1lbnQpLm9uKCdjbGljaycsIGZ1bmN0aW9uIChlKSB7XHJcbiAgICAgICAgdmFyICR0YXJnZXQgPSAkKGUudGFyZ2V0KTtcclxuICAgICAgICBpZiAoJHRhcmdldC5pcygnW2RhdGEtc2VhcmNoLXJlc3VsdF0nKSB8fCAkdGFyZ2V0LnBhcmVudHMoJ1tkYXRhLXNlYXJjaC1yZXN1bHRdJykubGVuZ3RoID4gMCkge1xyXG4gICAgICAgICAgICB2YXIgaWQgPSBzZWFyY2hSZXN1bHRzLmNsaWNrSGFuZGxlcigkdGFyZ2V0KTtcclxuICAgICAgICAgICAgaGVscGVyc18xLnNldENvbnRlbnRVcmwoaWQpO1xyXG4gICAgICAgICAgICBjb250ZW50LnNob3coaWQpO1xyXG4gICAgICAgICAgICAvLyBhZGQgc2Nyb2xsIHRvIGZvdW5kIGl0ZW1cclxuICAgICAgICAgICAgdmFyIHJlc3VsdElEID0gJHRhcmdldC5maW5kKCdbZGF0YS1yZXN1bHQtaWRdJykuYXR0cignZGF0YS1yZXN1bHQtaWQnKTtcclxuICAgICAgICAgICAgdmFyIG9mZnNldCA9ICQoaGVscGVyc18xLmNvbnRlbnRJdGVtKGlkKSkuZmluZChcIltkYXRhLXJlc3VsdC1pZD1cXFwiXCIgKyByZXN1bHRJRCArIFwiXFxcIl1cIikub2Zmc2V0KCkudG9wO1xyXG4gICAgICAgICAgICAkKCcuY29udGVudCcpLnNjcm9sbFRvcChvZmZzZXQgPiAzMDAgPyBvZmZzZXQgOiAwKTtcclxuICAgICAgICB9XHJcbiAgICAgICAgaWYgKCR0YXJnZXQucGFyZW50cygnW2RhdGEtc2VhcmNoLXJlc3VsdF0nKS5sZW5ndGggPT09IDAgJiYgISR0YXJnZXQuaXMoJ1tkYXRhLXNlYXJjaF0nKSkge1xyXG4gICAgICAgICAgICBzZWFyY2hSZXN1bHRzLnJlbW92ZSgpO1xyXG4gICAgICAgIH1cclxuICAgICAgICBpZiAoJHRhcmdldC5pcyhoZWxwZXJzXzEuc2VjdGlvbkl0ZW0oKSkpIHtcclxuICAgICAgICAgICAgZS5wcmV2ZW50RGVmYXVsdCgpO1xyXG4gICAgICAgICAgICBzdW1tYXJ5Lml0ZW1DbGlja0hhbmRsZXIoJHRhcmdldC5hdHRyKCdkYXRhLXNlY3Rpb24taXRlbScpKTtcclxuICAgICAgICAgICAgJCgnLmNvbnRlbnQnKS5zY3JvbGxUb3AoMCk7XHJcbiAgICAgICAgfVxyXG4gICAgICAgIGlmICgkdGFyZ2V0LmlzKCdbZGF0YS1wcmludF0nKSkge1xyXG4gICAgICAgICAgICB3aW5kb3cucHJpbnQoKTtcclxuICAgICAgICB9XHJcbiAgICAgICAgaWYgKCR0YXJnZXQuaXMoJ1tkYXRhLXNlYXJjaF0nKSkge1xyXG4gICAgICAgICAgICBzZWFyY2hCdG5IYW5kbGVyKCR0YXJnZXQpO1xyXG4gICAgICAgIH1cclxuICAgICAgICBpZiAoJHRhcmdldC5pcygnW2RhdGEtcmVzZXRdJykpIHtcclxuICAgICAgICAgICAgcmVzZXRCdG5IYW5kbGVyKCR0YXJnZXQpO1xyXG4gICAgICAgIH1cclxuICAgICAgICBpZiAoJHRhcmdldC5jbG9zZXN0KCdbZGF0YS1tZW51LWJ0bicpLmxlbmd0aCA+IDApIHtcclxuICAgICAgICAgICAgJCgnW2RhdGEtc3VtbWFyeV0nKS50b2dnbGVDbGFzcygnd3JhcHBlcl9fc3VtbWFyeV9hY3RpdmUnKTtcclxuICAgICAgICB9XHJcbiAgICB9KTtcclxuICAgICQoJ1tkYXRhLXNlYXJjaF0sIFtkYXRhLXByaW50XScpLm9uKFwibW91c2Vkb3duIG1vdXNldXBcIiwgZnVuY3Rpb24gKGUpIHtcclxuICAgICAgICAvLyBAdHMtaWdub3JlXHJcbiAgICAgICAgaWYgKGUuaGFuZGxlT2JqLnR5cGUgPT09IFwibW91c2V1cFwiKSB7XHJcbiAgICAgICAgICAgICQodGhpcykucmVtb3ZlQ2xhc3MoJ3ByZXNzZWQnKTtcclxuICAgICAgICAgICAgLy8gQHRzLWlnbm9yZVxyXG4gICAgICAgIH1cclxuICAgICAgICBlbHNlIGlmIChlLmhhbmRsZU9iai50eXBlID09PSBcIm1vdXNlZG93blwiKSB7XHJcbiAgICAgICAgICAgICQodGhpcykuYWRkQ2xhc3MoJ3ByZXNzZWQnKTtcclxuICAgICAgICB9XHJcbiAgICB9KTtcclxuICAgICQoJ1tkYXRhLXNlYXJjaC1maWVsZF0nKS5vbigna2V5dXAgaW5wdXQnLCBmdW5jdGlvbiAoZSkge1xyXG4gICAgICAgIHZhciB2YWwgPSAkKHRoaXMpLnZhbCgpO1xyXG4gICAgICAgIHNlYXJjaFJlc3VsdHMucmVtb3ZlKCk7XHJcbiAgICAgICAgcmVtb3ZlUHJldmlvdXNIaWdobGlnaHQoKTtcclxuICAgICAgICB2YWxpZGF0ZVNlYXJjaElucHV0KHZhbCk7XHJcbiAgICAgICAgaWYgKGUud2hpY2ggPT0gMTMpIHtcclxuICAgICAgICAgICAgJCgnW2RhdGEtc2VhcmNoXScpLnRyaWdnZXIoJ2NsaWNrJyk7XHJcbiAgICAgICAgfVxyXG4gICAgICAgIGVsc2UgaWYgKGUud2hpY2ggPT0gMjcpIHtcclxuICAgICAgICAgICAgJCgnW2RhdGEtcmVzZXRdJykudHJpZ2dlcignY2xpY2snKTtcclxuICAgICAgICB9XHJcbiAgICB9KTtcclxuICAgIGNyZWF0ZVNtYWxsSW1hZ2VzKCk7XHJcbn0pO1xyXG5mdW5jdGlvbiByZXNldEJ0bkhhbmRsZXIoJHRhcmdldCkge1xyXG4gICAgc2VhcmNoUmVzdWx0cy5yZW1vdmUoKTtcclxuICAgIHJlbW92ZVByZXZpb3VzSGlnaGxpZ2h0KCk7XHJcbiAgICAkKCdbZGF0YS1zZWFyY2gtZmllbGRdJykudmFsKCcnKTtcclxuICAgIHZhbGlkYXRlU2VhcmNoSW5wdXQoJCgnW2RhdGEtc2VhcmNoLWZpZWxkXScpLnZhbCgpKTtcclxufVxyXG5mdW5jdGlvbiBzZWFyY2hCdG5IYW5kbGVyKCR0YXJnZXQpIHtcclxuICAgIC8vIGlmIGluYWN0aXZlIC0gZG8gbm90aGluZ1xyXG4gICAgaWYgKCR0YXJnZXQuYXR0cignZGF0YS1pcy1hY3RpdmUnKSA9PT0gJ2ZhbHNlJylcclxuICAgICAgICByZXR1cm47XHJcbiAgICB2YXIgdmFsID0gJCgnW2RhdGEtc2VhcmNoLWZpZWxkXScpLnZhbCgpLnRvU3RyaW5nKCk7XHJcbiAgICBpZiAodmFsKSB7XHJcbiAgICAgICAgdmFyIHNhZmVJbnB1dCA9IHZhbC5yZXBsYWNlKC9bLiorP14ke30oKXxbXFxdXFxcXF0vZywgXCJcXFxcJCZcIik7XHJcbiAgICAgICAgcmVtb3ZlUHJldmlvdXNIaWdobGlnaHQoKTtcclxuICAgICAgICBzZWFyY2hSZXN1bHRzLnJlbmRlcihzZWFyY2hfMVtcImRlZmF1bHRcIl0oc2FmZUlucHV0LnRyaW0oKSkpO1xyXG4gICAgICAgIC8vIGZvcmNlIGJ1dHRvbiBub3QgdG8gcmVhY3QgdG8gY2xpY2sgZm9yIG9uZSBzZWNvbmRcclxuICAgICAgICAkdGFyZ2V0LmF0dHIoJ2RhdGEtaXMtYWN0aXZlJywgJ2ZhbHNlJyk7XHJcbiAgICAgICAgc2V0VGltZW91dChmdW5jdGlvbiAoKSB7XHJcbiAgICAgICAgICAgICR0YXJnZXQuYXR0cignZGF0YS1pcy1hY3RpdmUnLCAndHJ1ZScpO1xyXG4gICAgICAgIH0sIDEwMDApO1xyXG4gICAgfVxyXG59XHJcbjtcclxuZnVuY3Rpb24gY3JlYXRlU21hbGxJbWFnZXMoKSB7XHJcbiAgICAkKGhlbHBlcnNfMS5jb250ZW50SXRlbSgpICsgXCIgaW1nXCIpLm5vdCgnLmNvbnRlbnRfX25vdGVfX2ltZycpLmVhY2goZnVuY3Rpb24gKF9pLCBpbWcpIHtcclxuICAgICAgICB2YXIgc3JjID0gJChpbWcpLmF0dHIoJ3NyYycpO1xyXG4gICAgICAgIGlmIChzcmMuaW5kZXhPZignaW5saW5lJykgIT09IC0xIHx8IHNyYy5pbmRleE9mKCdpY29uJykgIT09IC0xKSB7XHJcbiAgICAgICAgICAgICQoaW1nKS5hZGRDbGFzcygnY29udGVudF9faW5saW5lLWltZycpO1xyXG4gICAgICAgIH1cclxuICAgICAgICBlbHNlIHtcclxuICAgICAgICAgICAgJChpbWcpLmFkZENsYXNzKFwiY29udGVudF9faW1nXCIpO1xyXG4gICAgICAgIH1cclxuICAgIH0pO1xyXG59XHJcbjtcclxuZnVuY3Rpb24gdmFsaWRhdGVTZWFyY2hJbnB1dCh2YWx1ZSkge1xyXG4gICAgLy8gaW5wdXQgaW4gZ2VuZXJhbCBhbGxvd2VkIGZvciBhbnkgbGV0dGVyIGZyb20gYW55IGxhbmcsIHNwYWNlIGFuZCBudW1iZXJzXHJcbiAgICAvLyB0aGlzIHJlZ2V4cCByZXR1cm5zIHRydWUgZm9yIGFueSBsZXR0ZXIgb3IgbnVtYmVyIGFuZCBzcGFjZSBvciBzZW1pY29sb25cclxuICAgIHZhciB3b3Jkc0FuZFNwYWNlID0gWFJlZ0V4cCgnKFxcXFxwe0x9ezIsfXxcXFxcZCspXFxcXCwqXFxcXHMqXFxcXDEqJyk7XHJcbiAgICAvLyB0aGlzIHJlZ2V4cCByZXR1cm5zIHRydWUgZm9yIGFueSBub24tbGV0dGVyIHN5bWJvbCBleGNlcHQgc3BhY2UgYW5kIG51bWJlciBvciBzZW1pY29sb25cclxuICAgIHZhciBzcGVjaWFsU3ltYm9scyA9IFhSZWdFeHAoXCJbXihcXFxccHtMfV5cXFxcc3xeXFxcXGR8XlxcXFwsfF5cXFxcLXxeXFxcXCcpXVwiKTtcclxuICAgIHdvcmRzQW5kU3BhY2UudGVzdCh2YWx1ZS50b1N0cmluZygpKSAmJiAhc3BlY2lhbFN5bWJvbHMudGVzdCh2YWx1ZS50b1N0cmluZygpKVxyXG4gICAgICAgID8gJCgnW2RhdGEtc2VhcmNoXScpLmF0dHIoJ2RhdGEtaXMtYWN0aXZlJywgJ3RydWUnKS5hZGRDbGFzcygnaGVhZGVyX19zZWFyY2gtYnRuX2FjdGl2ZScpXHJcbiAgICAgICAgOiAkKCdbZGF0YS1zZWFyY2hdJykuYXR0cignZGF0YS1pcy1hY3RpdmUnLCAnZmFsc2UnKS5yZW1vdmVDbGFzcygnaGVhZGVyX19zZWFyY2gtYnRuX2FjdGl2ZScpO1xyXG4gICAgdmFsdWUgPT09ICcnXHJcbiAgICAgICAgPyAkKCdbZGF0YS1yZXNldF0nKS5oaWRlKClcclxuICAgICAgICA6ICQoJ1tkYXRhLXJlc2V0XScpLnNob3coKTtcclxufVxyXG47XHJcbmZ1bmN0aW9uIHJlbW92ZVByZXZpb3VzSGlnaGxpZ2h0KCkge1xyXG4gICAgJCgnLmNvbnRlbnRfX3NlYXJjaC1yZXN1bHQnKS5lYWNoKGZ1bmN0aW9uIChpLCBlbGVtKSB7XHJcbiAgICAgICAgdmFyIHBhcmVudCA9IGVsZW0ucGFyZW50RWxlbWVudDtcclxuICAgICAgICAkKGVsZW0pLnJlcGxhY2VXaXRoKCQoZWxlbSkudGV4dCgpKTtcclxuICAgICAgICAvLyBhZnRlciByZW1vdmluZyBoaWdobGlnaHQtc3BhbiB0ZXh0LW5vZGVzIGFyZSBzcGxpdFxyXG4gICAgICAgIHBhcmVudC5ub3JtYWxpemUoKTtcclxuICAgIH0pO1xyXG59XHJcbi8vIG1pc3NpbmcgZm9yRWFjaCBvbiBOb2RlTGlzdCBmb3IgSUUxMVxyXG5pZiAod2luZG93Lk5vZGVMaXN0ICYmICFOb2RlTGlzdC5wcm90b3R5cGUuZm9yRWFjaCkge1xyXG4gICAgLy8gQHRzLWlnbm9yZVxyXG4gICAgTm9kZUxpc3QucHJvdG90eXBlLmZvckVhY2ggPSBBcnJheS5wcm90b3R5cGUuZm9yRWFjaDtcclxufVxyXG5pZiAoIUFycmF5LnByb3RvdHlwZS5maW5kKSB7XHJcbiAgICBBcnJheS5wcm90b3R5cGUuZmluZCA9IGZ1bmN0aW9uIChwcmVkaWNhdGUpIHtcclxuICAgICAgICBpZiAodGhpcyA9PSBudWxsKSB7XHJcbiAgICAgICAgICAgIHRocm93IG5ldyBUeXBlRXJyb3IoJ0FycmF5LnByb3RvdHlwZS5maW5kIGNhbGxlZCBvbiBudWxsIG9yIHVuZGVmaW5lZCcpO1xyXG4gICAgICAgIH1cclxuICAgICAgICBpZiAodHlwZW9mIHByZWRpY2F0ZSAhPT0gJ2Z1bmN0aW9uJykge1xyXG4gICAgICAgICAgICB0aHJvdyBuZXcgVHlwZUVycm9yKCdwcmVkaWNhdGUgbXVzdCBiZSBhIGZ1bmN0aW9uJyk7XHJcbiAgICAgICAgfVxyXG4gICAgICAgIHZhciBsaXN0ID0gT2JqZWN0KHRoaXMpO1xyXG4gICAgICAgIHZhciBsZW5ndGggPSBsaXN0Lmxlbmd0aCA+Pj4gMDtcclxuICAgICAgICB2YXIgdGhpc0FyZyA9IGFyZ3VtZW50c1sxXTtcclxuICAgICAgICB2YXIgdmFsdWU7XHJcbiAgICAgICAgZm9yICh2YXIgaSA9IDA7IGkgPCBsZW5ndGg7IGkrKykge1xyXG4gICAgICAgICAgICB2YWx1ZSA9IGxpc3RbaV07XHJcbiAgICAgICAgICAgIGlmIChwcmVkaWNhdGUuY2FsbCh0aGlzQXJnLCB2YWx1ZSwgaSwgbGlzdCkpIHtcclxuICAgICAgICAgICAgICAgIHJldHVybiB2YWx1ZTtcclxuICAgICAgICAgICAgfVxyXG4gICAgICAgIH1cclxuICAgICAgICByZXR1cm4gdW5kZWZpbmVkO1xyXG4gICAgfTtcclxufVxyXG4iLCJcInVzZSBzdHJpY3RcIjtcclxuZXhwb3J0cy5fX2VzTW9kdWxlID0gdHJ1ZTtcclxudmFyIGhlbHBlcnNfMSA9IHJlcXVpcmUoXCIuL2hlbHBlcnNcIik7XHJcbi8vIGZ1bmN0aW9uIGlzIGZpcmVkIGV2ZXJ5IHRpbWUgd2hlbiBoYXNoIGluIHVybCBjaGFuZ2VzXHJcbmZ1bmN0aW9uIHNob3coaWQpIHtcclxuICAgICQoaGVscGVyc18xLmNvbnRlbnRJdGVtKCkpLmhpZGUoKTtcclxuICAgIHZhciAkYWN0aXZlQ29udGVudCA9ICQoaGVscGVyc18xLmNvbnRlbnRJdGVtKGlkKSk7XHJcbiAgICB2YXIgJGFjdGl2ZVN1bW1hcnlJdGVtID0gJChoZWxwZXJzXzEuc2VjdGlvbkl0ZW0oaWQpKTtcclxuICAgIHZhciBPUEVOX0NMQVNTID0gXCJvcGVuZWRcIjtcclxuICAgIHZhciBOT19CT1JERVJfQ0xBU1MgPSBcImNvbnRlbnRfX3NlY3Rpb25fbm8tYm9yZGVyXCI7XHJcbiAgICBpZiAoJGFjdGl2ZUNvbnRlbnQuaXMoJ1tkYXRhLWNvbnRlbnQtbGV2ZWw9XCIzXCJdJykpIHtcclxuICAgICAgICAvLyBjb250ZW50IG9mIGxldmVsIDMgaXMgY2hpbGRyZW4gbm9kZSBvZiB0aGUgY29udGVudCBpdGVtIGxldmVsIDJcclxuICAgICAgICAvLyBzbyB3ZSBuZWVkIHRvIGhpZGUgb3RoZXIgbGV2ZWwgMyBpdGVtcyBpbiB0aGUgc2FtZSBjb250ZW50IGxldmVsIDJcclxuICAgICAgICAkYWN0aXZlQ29udGVudC5zaWJsaW5ncygpLmhpZGUoKTtcclxuICAgICAgICAvLyB3ZSBzaG91bGQgZGlzcGxheSBwYXJlbnQgd2hpY2ggaXMgbGV2ZWwgMixcclxuICAgICAgICAvLyBiZWNhdXNlIGlmIHBhcmVudCBub2RlIGlzIGhpZGRlbiAtIGFsbCBjaGlsZHJlbiBhcmUgaGlkZGVuIGFsc29cclxuICAgICAgICAkYWN0aXZlQ29udGVudC5wYXJlbnQoaGVscGVyc18xLmNvbnRlbnRJdGVtKCkpLnNob3coKTtcclxuICAgICAgICAvLyBzaG93IGNvbnRlbnQgYW5kIHJlbW92ZSBib3JkZXJcclxuICAgICAgICAkYWN0aXZlQ29udGVudC5zaG93KCkuYWRkQ2xhc3MoTk9fQk9SREVSX0NMQVNTKTtcclxuICAgIH1cclxuICAgIGVsc2Uge1xyXG4gICAgICAgIC8vIGFkZCBib3JkZXIgdG8gY2hpbGRyZW4gbGV2ZWwgM1xyXG4gICAgICAgICRhY3RpdmVDb250ZW50LmNoaWxkcmVuKFwiLlwiICsgTk9fQk9SREVSX0NMQVNTKS5yZW1vdmVDbGFzcyhOT19CT1JERVJfQ0xBU1MpO1xyXG4gICAgICAgIC8vIGlmIGl0IGlzIGEgbGV2ZWwgdHdvIGNvbnRlbnQgLSBpdCBjYW4gaGF2ZSBhIGxldmVsIDMgc2VjdGlvbnMsXHJcbiAgICAgICAgLy8gd2hpY2ggYXJlIHdlIHNob3VsZCBkaXNwbGF5IGFsc29cclxuICAgICAgICAkYWN0aXZlQ29udGVudC5zaG93KCkuY2hpbGRyZW4oKS5zaG93KCk7XHJcbiAgICB9XHJcbiAgICAvLyB3ZSByZW1vdmUgYWxsIGFjdGl2ZSBjbGFzc2VzIGZyb20gc3VtbWFyeSBpdGVtc1xyXG4gICAgJChoZWxwZXJzXzEuc2VjdGlvbigpKS5ub3QoJGFjdGl2ZVN1bW1hcnlJdGVtLnBhcmVudHMoKSkucmVtb3ZlQ2xhc3MoT1BFTl9DTEFTUyk7XHJcbiAgICAvLyB0b2dnbGUgb3BlbmVkIGFuZCBhY3RpdmUgY2xhc3Mgb2Ygc2VjdGlvbiBlbGVtXHJcbiAgICAkYWN0aXZlU3VtbWFyeUl0ZW0ucGFyZW50cyhoZWxwZXJzXzEuc2VjdGlvbigpKS5hZGRDbGFzcyhPUEVOX0NMQVNTKTtcclxufVxyXG5leHBvcnRzLnNob3cgPSBzaG93O1xyXG4iLCJcInVzZSBzdHJpY3RcIjtcclxuZXhwb3J0cy5fX2VzTW9kdWxlID0gdHJ1ZTtcclxuLyoqIFJldHVybiBkYXRhLXNlY3Rpb24gc2VsZWN0b3IgZm9yIGVsZW1lbnRzIGZyb20gc3VtbWFyeSAqL1xyXG5mdW5jdGlvbiBzZWN0aW9uKGxldmVsKSB7XHJcbiAgICBpZiAobGV2ZWwgPT09IHZvaWQgMCkgeyBsZXZlbCA9IG51bGw7IH1cclxuICAgIHJldHVybiBsZXZlbFxyXG4gICAgICAgID8gXCJbZGF0YS1zZWN0aW9uPVxcXCJcIiArIGxldmVsICsgXCJcXFwiXVwiXHJcbiAgICAgICAgOiAnW2RhdGEtc2VjdGlvbl0nO1xyXG59XHJcbmV4cG9ydHMuc2VjdGlvbiA9IHNlY3Rpb247XHJcbi8qKiBSZXR1cm5zIGRhdGEtc2VjdGlvbi1pdGVtIHNlbGVjdG9yIGZvciBzdW1tYXJ5IGl0ZW1zIChoZWFkaW5ncykgKi9cclxuZnVuY3Rpb24gc2VjdGlvbkl0ZW0oaWQpIHtcclxuICAgIGlmIChpZCA9PT0gdm9pZCAwKSB7IGlkID0gXCJcIjsgfVxyXG4gICAgcmV0dXJuIGlkID09PSBcIlwiXHJcbiAgICAgICAgPyAnW2RhdGEtc2VjdGlvbi1pdGVtXSdcclxuICAgICAgICA6IFwiW2RhdGEtc2VjdGlvbi1pdGVtPVxcXCJcIiArIGlkICsgXCJcXFwiXCI7XHJcbn1cclxuZXhwb3J0cy5zZWN0aW9uSXRlbSA9IHNlY3Rpb25JdGVtO1xyXG4vKiogUmV0dXJucyBkYXRhLWNvbnRlbnQgc2VsZWN0b3IgZm9yIHBhcnRzIG9mIGNvbnRlbnQgdW5kZXIgZ2l2ZW4gaWQgKi9cclxuZnVuY3Rpb24gY29udGVudEl0ZW0oaWQpIHtcclxuICAgIGlmIChpZCA9PT0gdm9pZCAwKSB7IGlkID0gXCJcIjsgfVxyXG4gICAgcmV0dXJuIGlkID09PSBcIlwiXHJcbiAgICAgICAgPyAnW2RhdGEtY29udGVudF0nXHJcbiAgICAgICAgOiBcIltkYXRhLWNvbnRlbnQ9XFxcIlwiICsgaWQgKyBcIlxcXCJdXCI7XHJcbn1cclxuZXhwb3J0cy5jb250ZW50SXRlbSA9IGNvbnRlbnRJdGVtO1xyXG4vKiogU2V0cyB3aW5kb3cgaGFzaCB0byBlbXVsYXRlIGRlZmF1bHQgYW5jaG9yIGxpbmtzIGJlaGF2aW9yICovXHJcbmZ1bmN0aW9uIHNldENvbnRlbnRVcmwoaWQpIHtcclxuICAgIHdpbmRvdy5sb2NhdGlvbi5oYXNoID0gXCIjXCIgKyBpZDtcclxuICAgIHJldHVybiB3aW5kb3cubG9jYXRpb24uaHJlZjtcclxufVxyXG5leHBvcnRzLnNldENvbnRlbnRVcmwgPSBzZXRDb250ZW50VXJsO1xyXG4vKiogUmV0dXJucyBzZWxlY3RlZCBjb250ZW50IG9yIGZpcnN0IHNlbGVjdGFibGUgKi9cclxuZnVuY3Rpb24gZ2V0U2VsZWN0ZWRDb250ZW50SWQoKSB7XHJcbiAgICByZXR1cm4gd2luZG93LmxvY2F0aW9uLmhhc2ggPT09IFwiXCJcclxuICAgICAgICA/ICQoc2VjdGlvbigyKSkuZmlyc3QoKS5jaGlsZHJlbihzZWN0aW9uSXRlbSgpKS5maXJzdCgpLmF0dHIoJ2RhdGEtc2VjdGlvbi1pdGVtJylcclxuICAgICAgICA6IHdpbmRvdy5sb2NhdGlvbi5oYXNoLnJlcGxhY2UoXCIjXCIsIFwiXCIpO1xyXG59XHJcbmV4cG9ydHMuZ2V0U2VsZWN0ZWRDb250ZW50SWQgPSBnZXRTZWxlY3RlZENvbnRlbnRJZDtcclxuIiwiXCJ1c2Ugc3RyaWN0XCI7XHJcbmV4cG9ydHMuX19lc01vZHVsZSA9IHRydWU7XHJcbnZhciBoZWxwZXJzXzEgPSByZXF1aXJlKFwiLi9oZWxwZXJzXCIpO1xyXG5mdW5jdGlvbiBzZWFyY2goaW5wdXQpIHtcclxuICAgIHZhciByZXN1bHRzID0gW107XHJcbiAgICAvLyBjaGVjayBjb250ZW50IHNlY3Rpb25zIGZvciBhIG1hdGNoZXNcclxuICAgICQoaGVscGVyc18xLmNvbnRlbnRJdGVtKCkpLmVhY2goZnVuY3Rpb24gKF9pLCBlbGVtKSB7XHJcbiAgICAgICAgZWxlbS5ub3JtYWxpemUoKTtcclxuICAgICAgICB2YXIgcmVnZXhwID0gbmV3IFJlZ0V4cChpbnB1dCArIFwiKD8hX3whLXwhLilcIiwgJ2lnJyk7XHJcbiAgICAgICAgdmFyIG1hdGNoZXMgPSAkKGVsZW0pLnRleHQoKS5tYXRjaChyZWdleHApO1xyXG4gICAgICAgIGlmIChtYXRjaGVzKSB7XHJcbiAgICAgICAgICAgIHZhciByZXN1bHRzQXJyID0gW107XHJcbiAgICAgICAgICAgIHNlYXJjaExhc3RFbGVtKGVsZW0sIHJlZ2V4cCwgcmVzdWx0c0Fycik7XHJcbiAgICAgICAgICAgIGlmIChyZXN1bHRzQXJyLmxlbmd0aCA+IDApIHtcclxuICAgICAgICAgICAgICAgIHJlc3VsdHNBcnIuZm9yRWFjaChmdW5jdGlvbiAocmVzdWx0KSB7XHJcbiAgICAgICAgICAgICAgICAgICAgdmFyICRlbGVtID0gcmVzdWx0WzBdO1xyXG4gICAgICAgICAgICAgICAgICAgIHZhciBpZCA9ICRlbGVtLmNsb3Nlc3QoJ1tkYXRhLWNvbnRlbnRdJykuYXR0cignZGF0YS1jb250ZW50Jyk7XHJcbiAgICAgICAgICAgICAgICAgICAgdmFyIGh0bWwgPSAkZWxlbS5odG1sKCk7IC8vY3JlYXRlSGlnaGxpZ2h0ZWRIdG1sKHJlc3VsdFswXSwgcmVzdWx0WzFdKTtcclxuICAgICAgICAgICAgICAgICAgICAvL2lmIGNvbnRlbnQgb2Ygbm90ZVxyXG4gICAgICAgICAgICAgICAgICAgIGlmICgkZWxlbS5wYXJlbnRzKCdkaXYuY29udGVudF9fbm90ZScpLmxlbmd0aCA+IDApIHtcclxuICAgICAgICAgICAgICAgICAgICAgICAgJGVsZW0gPSAkZWxlbS5wYXJlbnRzKCdkaXYuY29udGVudF9fbm90ZScpO1xyXG4gICAgICAgICAgICAgICAgICAgICAgICBodG1sID0gJGVsZW0uZ2V0KDApLm91dGVySFRNTDtcclxuICAgICAgICAgICAgICAgICAgICB9XHJcbiAgICAgICAgICAgICAgICAgICAgcmVzdWx0cy5wdXNoKGNyZWF0ZVJlc3VsdChpZCwgaHRtbCkpO1xyXG4gICAgICAgICAgICAgICAgfSk7XHJcbiAgICAgICAgICAgIH1cclxuICAgICAgICB9XHJcbiAgICB9KTtcclxuICAgIHJldHVybiByZW1vdmVEdXBsaWNhdGVzKHJlc3VsdHMpO1xyXG59XHJcbmV4cG9ydHNbXCJkZWZhdWx0XCJdID0gc2VhcmNoO1xyXG5mdW5jdGlvbiByZW1vdmVEdXBsaWNhdGVzKHJlc3VsdHMpIHtcclxuICAgIC8vIGhlcmUgd2Ugd2lsbCByZW1vdmUgYWxsIHJlc3VsdHMgd2l0aCBlcXVhbCB0ZXh0IGFuZC9vciBlcXVhbCBhbW91dCBvZiByZXN1bHRzIGhpZ2hsaWdodGVkXHJcbiAgICByZXN1bHRzLmZvckVhY2goZnVuY3Rpb24gKGVsZW0sIGkpIHtcclxuICAgICAgICB2YXIgJGN1cnJlbnQgPSAkKFwiPGRpdj5cIiArIChlbGVtLmh0bWwgKyBlbGVtLmNvbnRlbnRJRCkgKyBcIjwvZGl2PlwiKTtcclxuICAgICAgICB2YXIgZHVwbGljYXRlID0gcmVzdWx0cy5maW5kKGZ1bmN0aW9uIChpdGVtLCBqKSB7XHJcbiAgICAgICAgICAgIHZhciBpc1RleHRFcXVhbCA9ICQoXCI8ZGl2PlwiICsgKGl0ZW0uaHRtbCArIGl0ZW0uY29udGVudElEKSArIFwiPC9kaXY+XCIpLnRleHQoKSA9PT0gJGN1cnJlbnQudGV4dCgpO1xyXG4gICAgICAgICAgICB2YXIgaXNJdGVtSW5jbHVkZUN1cnJlbnRUZXh0ID0gJChcIjxkaXY+XCIgKyAoaXRlbS5odG1sICsgaXRlbS5jb250ZW50SUQpICsgXCI8L2Rpdj5cIikudGV4dCgpLmluZGV4T2YoJGN1cnJlbnQudGV4dCgpKSAhPT0gLTE7XHJcbiAgICAgICAgICAgIHJldHVybiAoaXNUZXh0RXF1YWwgfHwgaXNJdGVtSW5jbHVkZUN1cnJlbnRUZXh0KVxyXG4gICAgICAgICAgICAgICAgJiYgaSAhPT0gajtcclxuICAgICAgICB9KTtcclxuICAgICAgICBpZiAoZHVwbGljYXRlKSB7XHJcbiAgICAgICAgICAgIHZhciBhbW91bnRPZlJlc3VsdHMgPSAkY3VycmVudC5maW5kKCcuY29udGVudF9fc2VhcmNoLXJlc3VsdCcpLmxlbmd0aDtcclxuICAgICAgICAgICAgdmFyIGFtb3VudE9mUmVzdWx0SW5EdXBsaWNhdGUgPSAkKFwiPGRpdj5cIiArIGR1cGxpY2F0ZS5odG1sICsgXCI8L2Rpdj5cIikuZmluZCgnLmNvbnRlbnRfX3NlYXJjaC1yZXN1bHQnKS5sZW5ndGg7XHJcbiAgICAgICAgICAgIHZhciBsZXZlbCA9ICQoaGVscGVyc18xLnNlY3Rpb25JdGVtKGVsZW0uY29udGVudElEKSkucGFyZW50KCkuYXR0cignZGF0YS1zZWN0aW9uJyk7XHJcbiAgICAgICAgICAgIHZhciBsZXZlbE9mRHVwbGljYXRlID0gJChoZWxwZXJzXzEuc2VjdGlvbkl0ZW0oZHVwbGljYXRlLmNvbnRlbnRJRCkpLnBhcmVudCgpLmF0dHIoJ2RhdGEtc2VjdGlvbicpO1xyXG4gICAgICAgICAgICBpZiAoYW1vdW50T2ZSZXN1bHRzIDw9IGFtb3VudE9mUmVzdWx0SW5EdXBsaWNhdGUgJiYgbGV2ZWwgPj0gbGV2ZWxPZkR1cGxpY2F0ZSkge1xyXG4gICAgICAgICAgICAgICAgcmVzdWx0cy5zcGxpY2UoaSwgMSk7XHJcbiAgICAgICAgICAgIH1cclxuICAgICAgICAgICAgO1xyXG4gICAgICAgIH1cclxuICAgIH0pO1xyXG4gICAgcmV0dXJuIHJlc3VsdHM7XHJcbn1cclxuLy8gcmVjdXJzaXZlbHkgc2VhcmNoIGxhc3QgdGV4dCBub2RlIHRoYXQgY29udGFpbnMgc2VhcmNoIGlucHV0XHJcbi8vIGFuZCB3cml0ZXMgYWxsIG5vZGVzIHRvIHJlc3VsdHMgYXJyYXlcclxuZnVuY3Rpb24gc2VhcmNoTGFzdEVsZW0oZWxlbSwgcmVnZXhwLCByZXN1bHRzQXJyYXkpIHtcclxuICAgIHZhciBjaGlsZHJlbiA9IGVsZW0uY2hpbGROb2RlcztcclxuICAgIHZhciByZXN1bHQ7XHJcbiAgICBjaGlsZHJlbi5mb3JFYWNoKGZ1bmN0aW9uIChjaGlsZCwgaSkge1xyXG4gICAgICAgIC8vIEB0cy1pZ25vcmVcclxuICAgICAgICBpZiAoY2hpbGQucGFyZW50Tm9kZS5jbGFzc05hbWUgPT09ICdjb250ZW50X19zZWFyY2gtcmVzdWx0JylcclxuICAgICAgICAgICAgcmV0dXJuO1xyXG4gICAgICAgIGlmIChjaGlsZC5ub2RlTmFtZSAhPT0gXCIjdGV4dFwiKSB7XHJcbiAgICAgICAgICAgIHNlYXJjaExhc3RFbGVtKGNoaWxkLCByZWdleHAsIHJlc3VsdHNBcnJheSk7XHJcbiAgICAgICAgfVxyXG4gICAgICAgIGVsc2Uge1xyXG4gICAgICAgICAgICB2YXIgdGV4dCA9IGNoaWxkLnRleHRDb250ZW50O1xyXG4gICAgICAgICAgICB2YXIgbWF0Y2ggPSB0ZXh0Lm1hdGNoKHJlZ2V4cCk7XHJcbiAgICAgICAgICAgIGlmIChtYXRjaCkge1xyXG4gICAgICAgICAgICAgICAgdmFyICRwYXJlbnQgPSAkKGNoaWxkLnBhcmVudE5vZGUpO1xyXG4gICAgICAgICAgICAgICAgLy8gaGVyZSB3ZSB0YWtlIGEgcGFyZW50IG9mIHRleHQgbm9kZSwgbm90IGEgdGV4dCBvbmx5XHJcbiAgICAgICAgICAgICAgICAvLyB3ZSB3aWxsIG5lZWQgaXQgbGF0ZXIgZm9yIGhpZ2hsaWdodGluZyByZXN1bHQgb24gYSBwYWdlIFxyXG4gICAgICAgICAgICAgICAgLy9oaWdobGl0aW5nXHJcbiAgICAgICAgICAgICAgICBjaGlsZC5ub2RlVmFsdWUgPSB0ZXh0LnJlcGxhY2UocmVnZXhwLCBmdW5jdGlvbiAobWF0Y2gsIG9mZnNldCwgaW5wdXQpIHtcclxuICAgICAgICAgICAgICAgICAgICAvLyDQutC+0LvRhdC+0LfQvdGL0Lkg0YHQv9C+0YHQvtCxINC/0YDQvtCy0LXRgNC40YLRjCDQvdC1INC+0LHQtdGA0L3Rg9GCINC70Lgg0LzQsNGC0Ycg0LIg0YHQv9Cw0L0g0YEg0LrQu9Cw0YHRgdC+0LxcclxuICAgICAgICAgICAgICAgICAgICAvLyBjb250ZW50X19zZWFyY2gtcmVzdWx0XHJcbiAgICAgICAgICAgICAgICAgICAgaWYgKGlucHV0W29mZnNldCAtIDFdID09PSAnPicgJiYgaW5wdXRbb2Zmc2V0IC0gM10gPT09ICd0Jykge1xyXG4gICAgICAgICAgICAgICAgICAgICAgICByZXR1cm4gbWF0Y2g7XHJcbiAgICAgICAgICAgICAgICAgICAgfVxyXG4gICAgICAgICAgICAgICAgICAgIGVsc2Uge1xyXG4gICAgICAgICAgICAgICAgICAgICAgICByZXR1cm4gXCI8c3BhbiBjbGFzcz1cXFwiY29udGVudF9fc2VhcmNoLXJlc3VsdFxcXCIgZGF0YS1yZXN1bHQtaWQ9XFxcIlwiICsgTWF0aC5mbG9vcihNYXRoLnJhbmRvbSgpICogMTAwMCkgKyBcIlxcXCI+XCIgKyBtYXRjaCArIFwiPC9zcGFuPlwiO1xyXG4gICAgICAgICAgICAgICAgICAgIH1cclxuICAgICAgICAgICAgICAgIH0pO1xyXG4gICAgICAgICAgICAgICAgJHBhcmVudC5odG1sKHJlcGxhY2VUYWdFc2NhcGVDaGFyKCRwYXJlbnQuaHRtbCgpKSk7XHJcbiAgICAgICAgICAgICAgICBpZiAoJHBhcmVudC5pcygnYicpIHx8ICRwYXJlbnQuaXMoJ3N0cm9uZycpIHx8ICRwYXJlbnQuaXMoJ2EnKSkge1xyXG4gICAgICAgICAgICAgICAgICAgICRwYXJlbnQgPSAkcGFyZW50LnBhcmVudCgpO1xyXG4gICAgICAgICAgICAgICAgfVxyXG4gICAgICAgICAgICAgICAgO1xyXG4gICAgICAgICAgICAgICAgcmVzdWx0ID0gWyRwYXJlbnRdO1xyXG4gICAgICAgICAgICB9XHJcbiAgICAgICAgfVxyXG4gICAgfSk7XHJcbiAgICBpZiAocmVzdWx0KSB7XHJcbiAgICAgICAgcmVzdWx0c0FycmF5LnB1c2gocmVzdWx0KTtcclxuICAgIH1cclxufVxyXG5mdW5jdGlvbiByZXBsYWNlVGFnRXNjYXBlQ2hhcihlc2NhcGVkQ2hhcikge1xyXG4gICAgdmFyIHJlc3VsdCA9IGVzY2FwZWRDaGFyO1xyXG4gICAgcmVzdWx0ID0gcmVzdWx0LnJlcGxhY2UoL1xcJmd0XFw7L2csICc+Jyk7XHJcbiAgICByZXN1bHQgPSByZXN1bHQucmVwbGFjZSgvXFwmbHRcXDsvZywgJzwnKTtcclxuICAgIHJldHVybiByZXN1bHQ7XHJcbn1cclxuZnVuY3Rpb24gY3JlYXRlSGlnaGxpZ2h0ZWRIdG1sKCRlbGVtLCBtYXRjaGVzKSB7XHJcbiAgICB2YXIgcmVzdWx0SHRtbDtcclxuICAgIHZhciBzcmMgPSAkZWxlbS5maW5kKCdpbWcnKS5hdHRyKCdzcmMnKTtcclxuICAgIG1hdGNoZXMuZm9yRWFjaChmdW5jdGlvbiAobWF0Y2gpIHtcclxuICAgICAgICB2YXIgcmVnZXhwID0gbmV3IFJlZ0V4cChtYXRjaCwgJ2lnJyk7XHJcbiAgICAgICAgJGVsZW0uZmluZCgnaW1nJykuYXR0cignc3JjJywgJycpO1xyXG4gICAgICAgIHJlc3VsdEh0bWwgPSAkZWxlbS5odG1sKCkucmVwbGFjZShyZWdleHAsIGZ1bmN0aW9uIChtYXRjaCwgb2Zmc2V0LCBpbnB1dCkge1xyXG4gICAgICAgICAgICAvLyDQutC+0LvRhdC+0LfQvdGL0Lkg0YHQv9C+0YHQvtCxINC/0YDQvtCy0LXRgNC40YLRjCDQvdC1INC+0LHQtdGA0L3Rg9GCINC70Lgg0LzQsNGC0Ycg0LIg0YHQv9Cw0L0g0YEg0LrQu9Cw0YHRgdC+0LxcclxuICAgICAgICAgICAgLy8gY29udGVudF9fc2VhcmNoLXJlc3VsdFxyXG4gICAgICAgICAgICBpZiAoaW5wdXRbb2Zmc2V0IC0gMV0gPT09ICc+JyAmJiBpbnB1dFtvZmZzZXQgLSAzXSA9PT0gJ3QnKSB7XHJcbiAgICAgICAgICAgICAgICByZXR1cm4gbWF0Y2g7XHJcbiAgICAgICAgICAgIH1cclxuICAgICAgICAgICAgZWxzZSB7XHJcbiAgICAgICAgICAgICAgICByZXR1cm4gXCI8c3BhbiBjbGFzcz1cXFwiY29udGVudF9fc2VhcmNoLXJlc3VsdFxcXCIgZGF0YS1yZXN1bHQtaWQ9XFxcIlwiICsgTWF0aC5mbG9vcihNYXRoLnJhbmRvbSgpICogMTAwMCkgKyBcIlxcXCI+XCIgKyBtYXRjaCArIFwiPC9zcGFuPlwiO1xyXG4gICAgICAgICAgICB9XHJcbiAgICAgICAgfSk7XHJcbiAgICAgICAgJGVsZW0uaHRtbChyZXN1bHRIdG1sKTtcclxuICAgIH0pO1xyXG4gICAgJGVsZW0uZmluZCgnaW1nJykuYXR0cignc3JjJywgc3JjKTtcclxuICAgIGlmICgkZWxlbS5wYXJlbnRzKCdkaXYuY29udGVudF9fbm90ZScpLmxlbmd0aCA+IDApIHtcclxuICAgICAgICAkZWxlbSA9ICRlbGVtLnBhcmVudHMoJ2Rpdi5jb250ZW50X19ub3RlJyk7XHJcbiAgICAgICAgcmVzdWx0SHRtbCA9ICRlbGVtLmdldCgwKS5vdXRlckhUTUw7XHJcbiAgICB9XHJcbiAgICBlbHNlIHtcclxuICAgICAgICByZXN1bHRIdG1sID0gJGVsZW0uaHRtbCgpO1xyXG4gICAgfVxyXG4gICAgcmV0dXJuIHJlc3VsdEh0bWw7XHJcbn1cclxuO1xyXG5mdW5jdGlvbiBjcmVhdGVSZXN1bHQoaWQsIGh0bWwpIHtcclxuICAgIHJldHVybiB7XHJcbiAgICAgICAgY29udGVudElEOiBpZCxcclxuICAgICAgICBodG1sOiBodG1sXHJcbiAgICB9O1xyXG59XHJcbiIsIlwidXNlIHN0cmljdFwiO1xyXG5leHBvcnRzLl9fZXNNb2R1bGUgPSB0cnVlO1xyXG52YXIgaGVscGVyc18xID0gcmVxdWlyZShcIi4vaGVscGVyc1wiKTtcclxuLyoqIFJlbmRlciBzZWFyY2ggcmVzdWx0IHRvIGEgZHJvcGRvd24gKi9cclxuZnVuY3Rpb24gcmVuZGVyKHJlc3VsdHMpIHtcclxuICAgIHJlbW92ZSgpO1xyXG4gICAgaWYgKHJlc3VsdHMubGVuZ3RoID09PSAwKSB7XHJcbiAgICAgICAgdmFyIG1zZyA9ICQoJ1tkYXRhLW5vdGhpbmctZm91bmQtbXNnXScpLmF0dHIoJ2RhdGEtbm90aGluZy1mb3VuZC1tc2cnKTtcclxuICAgICAgICAkKFwiPGRpdiBjbGFzcz1cXFwiaGVhZGVyX19zZWFyY2hfX3Jlc3VsdHNfX2l0ZW1cXFwiPjxzcGFuIGNsYXNzPVxcXCJzZWFyY2gtaXRlbV9fcHJldmlld1xcXCI+XCIgKyBtc2cgKyBcIjwvc3Bhbj48L2Rpdj5cIilcclxuICAgICAgICAgICAgLmFwcGVuZFRvKCdbZGF0YS1zZWFyY2gtcmVzdWx0c10nKTtcclxuICAgIH1cclxuICAgIGVsc2Uge1xyXG4gICAgICAgIHJlc3VsdHMuZm9yRWFjaChmdW5jdGlvbiAocmVzdWx0KSB7XHJcbiAgICAgICAgICAgIF9yZW5kZXJJdGVtKHJlc3VsdCk7XHJcbiAgICAgICAgfSk7XHJcbiAgICB9XHJcbiAgICAkKCdbZGF0YS1zZWFyY2gtcmVzdWx0c10nKS5zaG93KCdmYXN0Jykuc2Nyb2xsVG9wKDApO1xyXG59XHJcbmV4cG9ydHMucmVuZGVyID0gcmVuZGVyO1xyXG47XHJcbi8qKiBSZXR1cm5zIGlkIHRvIHNob3cgKi9cclxuZnVuY3Rpb24gY2xpY2tIYW5kbGVyKCR0YXJnZXQpIHtcclxuICAgICQoJ1tkYXRhLXNlYXJjaC1yZXN1bHRzXScpLmhpZGUoJ2Zhc3QnKTtcclxuICAgIHJldHVybiAkdGFyZ2V0LmNsb3Nlc3QoJ1tkYXRhLXNlYXJjaC1yZXN1bHRdJykuYXR0cignZGF0YS1zZWFyY2gtcmVzdWx0Jyk7XHJcbn1cclxuZXhwb3J0cy5jbGlja0hhbmRsZXIgPSBjbGlja0hhbmRsZXI7XHJcbmZ1bmN0aW9uIF9yZW5kZXJJdGVtKHJlc3VsdCkge1xyXG4gICAgLy8gYXJyYXkgb2YgcGFyZW50cyBpZHMgaW4gc3VtbWFyeSBmb3IgYnJlYWRjcnVtYnNcclxuICAgIHZhciBwYXRocyA9IFtdO1xyXG4gICAgLy8gc3VtbWFyeSBpdGVtXHJcbiAgICB2YXIgJGl0ZW0gPSAkKFwiW2RhdGEtc2VjdGlvbi1pdGVtPVxcXCJcIiArIHJlc3VsdC5jb250ZW50SUQgKyBcIlxcXCJdXCIpO1xyXG4gICAgLy8gY3JlYXRlIGEgYnJlYWRjcnVtYnNcclxuICAgICRpdGVtLnBhcmVudHMoaGVscGVyc18xLnNlY3Rpb24oKSkuc2libGluZ3MoaGVscGVyc18xLnNlY3Rpb25JdGVtKCkpLmVhY2goZnVuY3Rpb24gKGksIGVsZW0pIHtcclxuICAgICAgICBwYXRocy5wdXNoKCQoZWxlbSkudGV4dCgpKTtcclxuICAgIH0pO1xyXG4gICAgLy8gbGFzdCBpdGVtIGluIGJyZWFkY3J1bWJzIC0gb3VyIHN1bW1hcnkgaXRlbSB0ZXh0XHJcbiAgICBwYXRocy5wdXNoKCRpdGVtLnRleHQoKSk7XHJcbiAgICAkKGl0ZW1UZW1wbGF0ZShyZXN1bHQuY29udGVudElELCBwYXRocy5qb2luKFwiID4gXCIpLCByZXN1bHQuaHRtbCkpLmFwcGVuZFRvKCdbZGF0YS1zZWFyY2gtcmVzdWx0c10nKTtcclxuICAgIGZ1bmN0aW9uIGl0ZW1UZW1wbGF0ZShpZCwgcGF0aFN0cmluZywgaHRtbCkge1xyXG4gICAgICAgIHJldHVybiBcIlxcbiAgICAgIDxkaXYgY2xhc3M9XFxcImhlYWRlcl9fc2VhcmNoX19yZXN1bHRzX19pdGVtIHNlYXJjaC1pdGVtXFxcIiBkYXRhLXNlYXJjaC1yZXN1bHQ9XFxcIlwiICsgaWQgKyBcIlxcXCI+XFxuICAgICAgICA8c3BhbiBjbGFzcz1cXFwic2VhcmNoLWl0ZW1fX3BhdGhcXFwiPlwiICsgcGF0aFN0cmluZyArIFwiPC9zcGFuPlxcbiAgICAgICAgPHAgY2xhc3M9XFxcInNlYXJjaC1pdGVtX19wcmV2aWV3XFxcIj5cIiArIGh0bWwgKyBcIjwvcD5cXG4gICAgICA8L2Rpdj5cIjtcclxuICAgIH1cclxufVxyXG47XHJcbi8qKiBSZW1vdmUgYWxsIGNoaWxkcmVuIGZyb20gYSBzZWFyY2ggcmVzdWx0cyBkcm9wZG93biAqL1xyXG5mdW5jdGlvbiByZW1vdmUoKSB7XHJcbiAgICAkKCdbZGF0YS1zZWFyY2gtcmVzdWx0c10nKS5oaWRlKCkuY2hpbGRyZW4oKS5yZW1vdmUoKTtcclxufVxyXG5leHBvcnRzLnJlbW92ZSA9IHJlbW92ZTtcclxuIiwiXCJ1c2Ugc3RyaWN0XCI7XHJcbmV4cG9ydHMuX19lc01vZHVsZSA9IHRydWU7XHJcbnZhciBoZWxwZXJzXzEgPSByZXF1aXJlKFwiLi9oZWxwZXJzXCIpO1xyXG4vKiogSGFuZGxlcyBjbGljayBvbiBzdW1tYXJ5IGl0ZW0sIGFuZCBjaGVja3MgaXMgdGhlcmUgYSBjb250ZW50IHVuZGVyIGl0ICovXHJcbmZ1bmN0aW9uIGl0ZW1DbGlja0hhbmRsZXIoaWQpIHtcclxuICAgIC8vIGlmIGNsaWNrZWQgaXRlbSBpcyBhbiBsZXZlbCAyIGl0ZW0gLSBpdCBjYW4gaGFzIG5vIGNvbnRlbnRcclxuICAgIGlmICgkKGhlbHBlcnNfMS5jb250ZW50SXRlbShpZCkpLmxlbmd0aCA9PT0gMCkge1xyXG4gICAgICAgIC8vIHNvIHdlIHNob3VsZCBzZWxlY3QgZmlyc3QgaW5uZXIgbGV2ZWwgMyBpdGVtIHdpdGggY29udGVudFxyXG4gICAgICAgIC8vIGlmIGFueSBpbm5lciBsZXZlbCAzIGl0ZW0gc2VsZWN0ZWQgLSBkbyBub3RoaW5nLCB3ZSBvayBcclxuICAgICAgICBpZiAoJChoZWxwZXJzXzEuc2VjdGlvbkl0ZW0oaWQpKS5wYXJlbnQoKS5oYXNDbGFzcygnb3BlbmVkJykpXHJcbiAgICAgICAgICAgIHJldHVybjtcclxuICAgICAgICAvLyBlbHNlIC0gd2UgZmluZCBmaXJzdCBpbm5lciBpdGVtIHdpdGggY29udGVudCBhbmQgc2VsZWN0IGl0IFxyXG4gICAgICAgIHZhciBuZXdJZCA9ICQoaGVscGVyc18xLnNlY3Rpb25JdGVtKGlkKSkuc2libGluZ3MoaGVscGVyc18xLnNlY3Rpb24oKSkuZmlyc3QoKS5jaGlsZHJlbihoZWxwZXJzXzEuc2VjdGlvbkl0ZW0oKSkuZmlyc3QoKS5hdHRyKCdkYXRhLXNlY3Rpb24taXRlbScpO1xyXG4gICAgICAgIGhlbHBlcnNfMS5zZXRDb250ZW50VXJsKG5ld0lkKTtcclxuICAgIH1cclxuICAgIGVsc2Uge1xyXG4gICAgICAgIGhlbHBlcnNfMS5zZXRDb250ZW50VXJsKGlkKTtcclxuICAgIH1cclxufVxyXG5leHBvcnRzLml0ZW1DbGlja0hhbmRsZXIgPSBpdGVtQ2xpY2tIYW5kbGVyO1xyXG4iXSwic291cmNlUm9vdCI6IiJ9