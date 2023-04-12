;(function() {
  'use strict'

  function findFirstMenu(target, root) {
    if (target == undefined) return
    if (target == root) return
    if (
      target.classList.contains('list_opentype')
    ) {
      return target
    }
    return findFirstMenu(target.parentElement, root)
  }

  function findThirdMenu(target, root) {
    if (target == undefined) return
    if (target == root) return
    if (target.tagName === 'A' && target.classList.contains('link_detailidx')) {
      return target
    }
    return findThirdMenu(target.parentElement, root)
  }
    
function findThirdMenu4(target, root) {
    if (target == undefined) return
    if (target == root) return
    if (target.classList.contains('list2_menu')) {
      return target
    }
    return findThirdMenu4(target.parentElement, root)
  }


  function findMenuRoot(target, root) {
    if (target == undefined) return
    if (target == root) return
    if (target.classList.contains('list_opentype')) return target
    return findMenuRoot(target.parentElement, root)
  }
    
    
    
    

  function selectThirdMenu(selectedMenu, root) {
    const menus = root.querySelectorAll('a.link_detailidx')
    menus.forEach(function(menu) {
      menu.classList.remove('on')
    })
    selectedMenu && selectedMenu.classList.add('on')
  }

 function selectThirdMenu2(selectedMenu, root) {
    const menus2 = root.querySelectorAll('ul.list_opentype')
    menus2.forEach(function(menu2) {
      menu2.classList.remove('open_on')
    })
    selectedMenu && selectedMenu.classList.add('open_on')
  }
    
function selectThirdMenu4(selectedMenu, root) {
    const menus4 = root.querySelectorAll('li.list2_menu')
    const menus = root.querySelectorAll('a.link_detailidx')
    menus4.forEach(function(menu4) {
      menu4.classList.remove('on')
    })
    

    selectedMenu && selectedMenu.classList.add('on')
  }
    
    

  window.addEventListener('DOMContentLoaded', function() {
    const docsRoot = document.querySelector('#docsRoot')
    const menuRoot = document.querySelector('#mFeature.menu_side')
    const menuArea = menuRoot.querySelector('.area_sidemenu')

    function initMenu() {
      menuArea.addEventListener('click', function(e) {
        const menu1 = findFirstMenu(e.target, menuArea)
        
        if (menu1) {
            
                if ($(e.target.parentElement.parentElement.parentElement).hasClass("open_on")) { 
                    //$(e.target.parentElement.parentElement.parentElement).removeClass("open_on");
                } else { 
                    selectThirdMenu2(menu1, menuArea)
                }

        }

        const menu3 = findThirdMenu(e.target, menuArea)
        if (menu3) {
          docsRoot.classList.remove('menu_on')
          menuRoot.classList.remove('menuside_on')
          selectThirdMenu(menu3, menuArea)
        }
          
        const menu4 = findThirdMenu4(e.target, menuArea)
        if (menu4) {
          docsRoot.classList.remove('menu_on')
          menuRoot.classList.remove('menuside_on')
          selectThirdMenu4(menu4, menuArea)
        }
          
      })
    }

    function initScrollMenuSelector() {
        /*
      const getScrollHeight = () => {
        const body = document.body
        const html = document.documentElement

        return Math.max(
          body.scrollHeight,
          body.offsetHeight,
          html.clientHeight,
          html.scrollHeight,
          html.offsetHeight
        )
      }
      */

      const getClientHeight = () => {
        const html = document.documentElement
        return html.clientHeight || window.innerHeight
      }

      function selectMenuFromTitle(title) {
        const selectedMenu = menuArea.querySelector(
          `a.link_detailidx[href='#${title.id}']`
        )
        return selectedMenu
      }

      window.addEventListener(
        'scroll',
        function() {
          //var scrollPosition = window.pageYOffset
         // var offsetHeight = getClientHeight()
          //var scrollHeight = getScrollHeight()
          var titleList = document.querySelectorAll(
            '#mArticle .heading-offset-anchor'
          )
          titleList.forEach(function(title) {
              /*
            if (scrollPosition >= title.offsetTop) {
              const selectedMenu = selectMenuFromTitle(title)
              return selectThirdMenu(selectedMenu, menuArea)
            }
            */
          })

          // end
            /*
          if (offsetHeight + scrollPosition == scrollHeight) {
            const lastTitle = titleList[titleList.length - 1]
            const selectedMenu = selectMenuFromTitle(lastTitle)
            return selectThirdMenu(selectedMenu, menuArea)
          }
          */
        },
        true
      )
    }

    function initResponsivePCMenuBtn() {
      const menuOpenBtn = document.querySelector('button.btn_menuopen')
      const menuCloseBtn = document.querySelector('button.btn_menuclose')

      menuOpenBtn.addEventListener('click', function(e) {
        e.stopPropagation()
        e.preventDefault()
        docsRoot.classList.add('menu_on')
      })
      menuCloseBtn.addEventListener('click', function(e) {
        e.stopPropagation()
        e.preventDefault()
        docsRoot.classList.remove('menu_on')
      })
    }

    function initMobileMenuBtn() {
      const menuToggleBtn = menuRoot.querySelector('button.btn_menutoggle')
    
      menuToggleBtn.addEventListener('click', function(e) {
        e.stopPropagation()
        e.preventDefault()
        menuRoot.classList.toggle('menuside_on')
        handleMobileMenuClick()
      })

      initMobileMenuScrollTop()
    }

    function handleMobileMenuClick() {
      // const opened = menuRoot.classList.contains('menuside_on')
      initMobileMenuScrollTop(menuRoot)
    }

    function initMobileMenuScrollTop() {
      const innerMenu = menuRoot.querySelector('.inner_side')
      const selected = menuRoot.querySelector('.list_defaulttype > li.on')
      const selectedBranch = closestElement(selected, 'list_opentype')
      if (!innerMenu || !selected || !selectedBranch) return

      innerMenu.scrollTop = selected.offsetTop + selectedBranch.offsetTop
    }

    function closestElement(el, className) {
      if (!el) return null
      if (el.closest) return el.closest('.' + className)

      if (el.classList.contains(className)) return el
      else return closestElement(el.parentElement, className)
    }

    initMenu()
    initScrollMenuSelector()
    initResponsivePCMenuBtn()
    initMobileMenuBtn()
  })
})()
