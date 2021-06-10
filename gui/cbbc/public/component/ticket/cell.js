class Cell extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
    this.initData = this.initData.bind(this)
    this.handleChange = this.handleChange.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.handleLayout = this.handleLayout.bind(this)
    this.initInputData = this.initInputData.bind(this)
  }
  
  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).draggable()
  }
  
  handleChange() {
    var {name, val, no, states, userId, algoName, obj} = this.initData()
    
    // 1.0 輪證
    if (name == 'wnt') {
      // 1.1 輸入中
      if (val.length<=obj.wnt.code.len) {
        var layout = obj.config.layout
        obj = this.props.getInstance()
        obj.config.layout = layout
        obj.wnt.code.code = val.replace(/[^0-9]/g, '')
      }
      // 1.2 已完成輸入
      if (val && val.length==obj.wnt.code.len) {
        var command = {cmd: 'get_warrant_detail', code: parseInt(val), algo_name: algoName, id: userId, no: no}
        sendWebsocket(JSON.stringify(command))
      }
    }
    
    // 2.0
    var intUpdatePrice = null
    if (name == 'updatePrice2') {
      if (obj.updatePrice.isUpdate2 == true)
        obj.updatePrice.isUpdate2 = false
      else if (obj.updatePrice.isUpdate2 == false) {
        /*obj.wnt.code.status1 = 'aTrack'
        obj.updatePrice.isUpdate2 = true
        intUpdatePrice = setInterval(() => {
          // 2.1
          if (obj.updatePrice.isUpdate2 == false)
            clearInterval(intUpdatePrice)
          // 2.2
          var command = {cmd: 'get_warrant_detail', code: parseInt(val), algo_name: algoName, id: userId, no: no}
          sendWebsocket(JSON.stringify(command))
          states.cells[no] = obj
          this.props.setStates({states: obj})
        }, 3000);*/
      }
    }
    
    states.cells[no] = obj
    this.props.setStates({states: obj})
  }
  
  // 输入框
  initInputData(obj, no, userId, algoName, cmd=null, isUseStoploss=null) {
    var wnt = $.extend(true, {}, obj.wnt),
        stock = $.extend(true, {}, obj.stock),
        data = {}
    // 輪
    data.code = parseInt(wnt.code.code)
    data.buy_price = formatLongV2(wnt.buy.price)
    data.sell_price = formatLongV2(wnt.sell.price)
    data.buy_qty = formatLongV2(formatInputUnit(wnt.buy.qty, true))
    data.stoploss = formatLongV2(wnt.stopLoss.price)
    if (wnt.position) {
      data.position = formatLongV2(formatInputUnit(wnt.position, true))
      data.sell_qty = data.position
    }
    else {
      data.position = 0
      data.sell_qty = formatLongV2(formatInputUnit(wnt.sell.qty, true))
    }
    
    // 正股
    data.ucode = parseInt(stock.code.code)
    data.buy_trriger = formatLongV2(stock.buy.price)
    data.sell_trriger = formatLongV2(stock.sell.price)
    data.early_buy_qty = formatLongV2(formatInputUnit(stock.buy.qty, true))
    data.early_sell_qty = formatLongV2(formatInputUnit(stock.sell.qty, true))
    data.ratio_buy = formatLongV2(stock.buy.ratio)
    data.ratio_sell = formatLongV2(stock.sell.ratio)
    // 通用
    data.ref = setNo(no)
    // 
    data.spread = getSpread(wnt.buy.price)
    data.bottom_price = formatLongV2(0.000)
    data.ceiling_price = formatLongV2(0.000)
    
    if (cmd==null) data.cmd = null
    else if (cmd=='set') {
      data.command = 
      {cmd: "set", type: "bull", warrant_code: data.code, underlying_code: data.ucode, 
      buy_trriger: data.buy_trriger, sell_trriger: data.sell_trriger, 
      buy_price: data.buy_price, sell_price: data.sell_price, 
      bottom_price: data.bottom_price, ceiling_price: data.ceiling_price,
      auto_buy_quantity: data.buy_qty, 
      early_buy_qty: data.early_buy_qty, early_sell_qty: data.early_sell_qty, 
      ratio_buy: data.ratio_buy, ratio_sell: data.ratio_sell, 
      id: userId, action: null, algo_name: algoName, ref: data.ref, position: 0}
    }
    return data
  }
  
  handleClick() {
    var {name, val, no, states, userId, algoName, obj} = this.initData()
    var wnt = $.extend(true, {}, obj.wnt),
        stock = $.extend(true, {}, obj.stock)
    
    // 1.0 输入框
    var inputData = this.initInputData(obj, no, userId, algoName, null, obj.wnt.stopLoss.status)
    var {/*輪*/code, buy_price, sell_price, buy_qty, sell_qty, stoploss, position,/*正股*/ucode, buy_trriger, sell_trriger, early_buy_qty, early_sell_qty, ratio_buy, ratio_sell, /*通用*/ref} = inputData
    
    // 2.0 前置檢查
    var isErrorWntBuy = false, isErrorWntSell = false, isErrorStockBuy = false, isErrorStockSell = false, isErrorStoploss = false
    if (!buy_price || buy_price==0)
      isErrorWntBuy = true, obj.wnt.msg.unshift(getTime()+' Warrant buy price cannot be null or 0.')
    else if (!buy_qty || buy_qty==0)
      isErrorWntBuy = true, obj.wnt.msg.unshift(getTime()+' Warrant buy qty cannot be null or 0.')
    
    if (!sell_price || sell_price==0)
      isErrorWntSell = true, obj.wnt.msg.unshift(getTime()+' Warrant sell price cannot be null or 0.')
    else if (!sell_qty || sell_qty==0)
      isErrorWntSell = true, obj.wnt.msg.unshift(getTime()+' Warrant sell qty cannot be null or 0.')
    
    if (!buy_trriger || buy_trriger==0)
      isErrorStockBuy = true, obj.wnt.msg.unshift(getTime()+' Stock trigger buy price cannot be null or 0.')
    else if (early_buy_qty<0)
      isErrorStockBuy = true, obj.wnt.msg.unshift(getTime()+' Stock trigger buy qty cannot be lsee than 0.')
    else if (ratio_buy<0)
      isErrorStockBuy = true, obj.wnt.msg.unshift(getTime()+' Stock trigger buy ratio cannot be less than 0.')
    
    if (!sell_trriger || sell_trriger==0)
      isErrorStockSell = true, obj.wnt.msg.unshift(getTime()+' Stock trigger sell price cannot be null or 0.')
    else if (early_sell_qty<0)
      isErrorStockSell = true, obj.wnt.msg.unshift(getTime()+' Stock trigger sell qty cannot be lsee than 0.')
    else if (ratio_sell<0)
      isErrorStockSell = true, obj.wnt.msg.unshift(getTime()+' Stock trigger sell ratio cannot be lsee than 0.')
    
    if (!stoploss || stoploss==0)
      isErrorStoploss = true, obj.wnt.msg.unshift(getTime()+' Warrant stoploss cannot be null or 0.')
    
    // 3.0 初始化
    if (name == 'wntBuy' || name == 'wntSell' || name == 'pair' || name == 'stoploss' || name == 'stock.action4') {
      if (wnt.sell.status == 'open' || wnt.buy.status == 'open') {
        var command4 = this.initInputData(obj, no, userId, algoName, 'set', false).command
        command4.action = 'STOP'
        sendWebsocket(JSON.stringify(command4))
      }
      else if (wnt.stopLoss.status == 'start' && stock.action1 == 'start') {
        var command5 = {cmd: "cancel", id: userId, algo_name: algoName, ref: setNo(no)}
        sendWebsocket(JSON.stringify(command5))
      }
      obj.stock.action2 = undefined
      obj.stock.action3 = undefined
    }
    
    // 3.1 買賣
    if (name == 'wntBuy' && wnt.buy.status != 'open') obj.wnt.buy.status = 'open', obj.wnt.stopLoss.status = 'stop'
    else if (name == 'wntSell' && wnt.sell.status != 'open') obj.wnt.sell.status = 'open', obj.wnt.stopLoss.status = 'stop'
    else if (name == 'wntBuy' && wnt.buy.status == 'open') obj.wnt.buy.status = 'close' 
    else if (name == 'wntSell' && wnt.sell.status == 'open') obj.wnt.sell.status = 'close'
    else if (name == 'pair') obj.wnt.buy.status = 'open', obj.wnt.sell.status = 'open', obj.wnt.stopLoss.status = 'stop'
    
    // 3.2 現價買入 / 賣出
    else if (name == 'stock.action4' && ((wnt.buy.status == 'open' && !isErrorWntBuy) || (wnt.sell.status == 'open' && !isErrorWntSell))) {
      obj.stock.action4 = 'start'
      if (position<=0)
        var command = {cmd: 'force_buy', quantity: buy_qty, id: userId, algo_name: algoName, ref: ref, price: buy_price}
      else
        var command = {cmd: 'force_sell', quantity: sell_qty, id: userId, algo_name: algoName, ref: ref, price: sell_price}
      sendWebsocket(JSON.stringify(command))
      
      obj.wnt.buy.status = undefined
      obj.wnt.sell.status = undefined
      obj.wnt.stopLoss.status = undefined
    }
    
    // 4.0 止蝕
    else if (name == 'stoploss' && !isErrorStoploss && position<=0) {
      obj.wnt.msg.unshift(getTime()+' Cannot Stoploss. Do not have position.')
    }
    else if (name == 'stoploss' && !isErrorStoploss && (!obj.wnt.stopLoss.status || obj.wnt.stopLoss.status=='stop')) {
      obj.wnt.sell.status = 'close'
      obj.wnt.buy.status = 'close'
      obj.wnt.stopLoss.status = 'start'
    }
    else if (name == 'stoploss' && !isErrorStoploss && obj.wnt.stopLoss.status=='start') {
      obj.wnt.stopLoss.status = 'stop'
    }
    
    // 5.0 開啟 交易策略
    var command1 = this.initInputData(obj, no, userId, algoName, 'set', obj.wnt.stopLoss.status).command
    if (name=='stock.action1' && (isErrorWntBuy || isErrorWntSell || isErrorStockBuy || isErrorStockSell))
      obj.stock.action1 = 'fail'
    else if (name=='stock.action1' && (!stock.action1 || stock.action1=='stop') && !isErrorWntBuy && !isErrorWntSell && !isErrorStockBuy && !isErrorStockSell) {
      if (obj.wnt.buy.status == 'open' && obj.wnt.sell.status == 'open' && position <= 0)
        command1.action = 'AUTO'
      else if (obj.wnt.buy.status == 'open' && obj.wnt.sell.status == 'open' && position > 0)
        command1.action = 'SELL'
      else if (obj.wnt.buy.status == 'open')
        command1.action = 'BUY'
      else if (obj.wnt.sell.status == 'open' && position > 0)
        command1.action = 'SELL'
      else if (obj.wnt.sell.status == 'open' && position <= 0)
        command1.action = 'NOCHANGE', obj.wnt.msg.unshift(getTime()+' Cannot start algo. Do not have position.')
      if (command1.action)
        sendWebsocket(JSON.stringify(command1))
      
      if (obj.wnt.stopLoss.status=='start') {
        var command6 = {cmd: 'limit_sell', price: stoploss, quantity: position, id: userId, algo_name: algoName, ref: setNo(no)}
        sendWebsocket(JSON.stringify(command6))
      }
    }
    else if (name=='stock.action1' && stock.action1 && (stock.action1=='start' || stock.action1=='fail')) {
      if (wnt.buy.status == 'open' || wnt.sell.status == 'open') {
        command1.action = 'STOP'
        sendWebsocket(JSON.stringify(command1))
      }
      else if (wnt.stopLoss.status == 'start' && stock.action1 == 'start') {
        var command5 = {cmd: "cancel", id: userId, algo_name: algoName, ref: setNo(no)}
        sendWebsocket(JSON.stringify(command5))
      }
      
      obj.wnt.buy.status = undefined
      obj.wnt.sell.status = undefined
      obj.wnt.stopLoss.status = undefined
      
      obj.stock.action2 = undefined
      obj.stock.action3 = undefined
    }
    else if (name=='stock.action2' && stock.action1 && stock.action1=='start' && (wnt.sell.status == 'open' || wnt.buy.status == 'open')) {
      if (!stock.action2) {
        obj.stock.action2 = 'start'
        command1.action = 'NOCHANGE'
        sendWebsocket(JSON.stringify(command1))
      }
      else {
        obj.stock.action2 = undefined
        //
        if (wnt.sell.status == 'open' && wnt.buy.status == 'open')
          command1.action = 'AUTO'
        else if (wnt.buy.status == 'open')
          command1.action = 'BUY'
        else if (wnt.sell.status == 'open')
          command1.action = 'SELL'
        sendWebsocket(JSON.stringify(command1))
      }
    }
    else if (name=='stock.action3' && stock.action1 && stock.action1=='start' && position>0 && wnt.stopLoss.status=='start') {
      if (!stock.action3)
        obj.stock.action3 = 'start'
      else {
        obj.stock.action3 = undefined
        var command1 = {cmd: 'limit_modify', price: stoploss, quantity: position, id: userId, algo_name: algoName, ref: setNo(no)}
        sendWebsocket(JSON.stringify(command1))
      }
    }
    
    // 6.0 正股 觸買
    if (name == 'stock.status2') {
      obj.stock.status1 = 'close', obj.stock.status2 = 'open', obj.stock.status3 = 'close', obj.stock.status4 = 'close'
      obj.stock.buy.ratio = 0
      obj.stock.sell.ratio = 0
    }
    else if (name == 'stock.status4') {
      obj.stock.status1 = 'close', obj.stock.status2 = 'close', obj.stock.status3 = 'close', obj.stock.status4 = 'open'
      obj.stock.buy.qty = 0
      obj.stock.sell.qty = 0
    }
    
    if (name == 'stock.status2' || name == 'stock.status4') {
      var command2 = this.initInputData(obj, no, userId, algoName, 'set', obj.wnt.stopLoss.status).command
      command2.action = 'NOCHANGE'
      sendWebsocket(JSON.stringify(command2))
    }
    
    // 7.0 track
    if (name == 'nTrack' || name == 'aTrack' || name == 'xTrack') {
      obj.wnt.code.status1 = name
      if (name == 'nTrack') obj.stock.nTrack = 'start'
      else if (name == 'aTrack') obj.stock.aTrack = 'start'
      else if (name == 'xTrack') obj.stock.xTrack = 'start'
      
      var command = {cmd: 'get_warrant_detail', code: parseInt(code), algo_name: algoName, id: userId, no: no}
      sendWebsocket(JSON.stringify(command))
      
      /*if (wnt.sell.status == 'open' || wnt.buy.status == 'open') {
        if (obj.stock.action1 == 'start')
          command1.action = 'STOP'
        else
          command1.action = 'NOCHANGE'
        sendWebsocket(JSON.stringify(command1))
      }*/
      
      function initStatus(obj) {
        obj.wnt.buy.status = undefined
        obj.wnt.sell.status = undefined
        obj.wnt.stopLoss.status = undefined
        obj.stock.action2 = undefined
        obj.stock.action3 = undefined
        return obj
      }
      
      if (wnt.stopLoss.status == 'start' && stock.action1 == 'start') {
        var command5 = {cmd: "cancel", id: userId, algo_name: algoName, ref: setNo(no)}
        sendWebsocket(JSON.stringify(command5))
        obj = initStatus(obj)
      }
      else if (!(obj.stock.action2 == 'start' || obj.stock.action3 == 'start')) {
        command1.action = 'STOP'
        sendWebsocket(JSON.stringify(command1))
        obj = initStatus(obj)
      }
    }
    
    // 8.0
    if (name == 'updatePrice1') {
      obj.wnt.code.status1 = 'aTrack'
      obj.stock.aTrack = 'start'
      obj.updatePrice.isUpdate1 = true
      var command = {cmd: 'get_warrant_detail', code: parseInt(code), algo_name: algoName, id: userId, no: no}
      sendWebsocket(JSON.stringify(command))
    }
    
    states.cells[no] = obj
    this.props.setStates({states: obj})
  }
  
  // 前置攔置器
  initData() {
    var states = this.props.getStates()
    return {
      name: event.target.name,
      val: formatInput(event.target.value),
      no: this.props.no,
      states: states,
      userId: parseInt(states.userId),
      algoName: states.modules.call,
      obj: $.extend(true, {}, states.cells[this.props.no])
    }
  }
  
  handleLayout() {
    var name = event.target.name,
        no = this.props.no,
        states = this.props.getStates(),
        obj = $.extend(true, {}, states.cells[this.props.no])
    
    var size = ['normal', 'large', 'xlarge']
    var idx = size.indexOf(obj.config.layout)

    if (name == 'add' && idx < size.length-1)
      obj.config.layout = size[idx+1]
    else if (name == 'subtract' && idx > 0)
      obj.config.layout = size[idx-1]
    
    states.cells[no] = obj
    this.props.setStates({states: obj})
  }
  
  render() {
    var wnt = $.extend(true, {}, this.props.data.wnt)
    var stock = $.extend(true, {}, this.props.data.stock)
    var config = $.extend(true, {}, this.props.data.config)
    
    // 1.0 輪證
    var isDisable = true
    if (wnt.code.status=='error') isDisable = true
    else if (wnt.code.status=='success') isDisable = false
    
    // 1.1 輪證输入框
    var isWntDisable = isDisable,
        cssWnt = ''
    if (!stock.action1 && !wnt.code.status)
      cssWnt = '', isWntDisable = false
    else if (wnt.code.status=='error')
      cssWnt = 'is-invalid', isWntDisable = false
    else if (stock.action1 == 'start' && wnt.code.status=='success')
      cssWnt = 'is-valid', isWntDisable = true
    else if (stock.action1 == 'stop' && wnt.code.status=='success')
      cssWnt = 'is-valid', isWntDisable = false
    else if (wnt.code.status=='success')
      cssWnt = 'is-valid', isWntDisable = false
    
    if (parseFloat(wnt.position)>0)
      isWntDisable = true
    
    // 2.0 錯誤信息
    var htmlMsg = ''
    for (var msg of wnt.msg)
      htmlMsg += msg+'\r\n'
    
    // 3.0 報價
    var isShowStockPriceTable = false, isShowWntPriceTable = false
    if (stock.priceTable.ask.ratio && stock.priceTable.bid.ratio) isShowStockPriceTable = true
    if (wnt.priceTable.ask.ratio && wnt.priceTable.bid.ratio) isShowWntPriceTable = true
    // 4.0 輪
    if (wnt.priceTable.ask.price) wnt.priceTable.ask.price = parseFloat(wnt.priceTable.ask.price).toFixed(3)
    if (wnt.priceTable.bid.price) wnt.priceTable.bid.price = parseFloat(wnt.priceTable.bid.price).toFixed(3)
    if (wnt.priceTable.ask.qty) wnt.priceTable.ask.qty = formatInputUnit(wnt.priceTable.ask.qty, false)
    if (wnt.priceTable.bid.qty) wnt.priceTable.bid.qty = formatInputUnit(wnt.priceTable.bid.qty, false)
    if (wnt.priceTable.ask.ratio) wnt.priceTable.ask.ratio = parseFloat(wnt.priceTable.ask.ratio).toFixed(2)
    if (wnt.priceTable.bid.ratio) wnt.priceTable.bid.ratio = parseFloat(wnt.priceTable.bid.ratio).toFixed(2)
    // 5.0 正股
    var digital = 2
    if (isETF(stock.code.code)) digital = 3
    if (stock.priceTable.ask.price) stock.priceTable.ask.price = parseFloat(stock.priceTable.ask.price).toFixed(digital)
    if (stock.priceTable.bid.price) stock.priceTable.bid.price = parseFloat(stock.priceTable.bid.price).toFixed(digital)
    if (stock.priceTable.ask.qty) stock.priceTable.ask.qty = formatInputUnit(stock.priceTable.ask.qty, false)
    if (stock.priceTable.bid.qty) stock.priceTable.bid.qty = formatInputUnit(stock.priceTable.bid.qty, false)
    if (stock.priceTable.ask.ratio) stock.priceTable.ask.ratio = parseFloat(stock.priceTable.ask.ratio).toFixed(2)
    if (stock.priceTable.bid.ratio) stock.priceTable.bid.ratio = parseFloat(stock.priceTable.bid.ratio).toFixed(2)
    
    // 6.0 輪證標題
    var title = 'Warrant '+this.props.no.toString()
    var cssTitle = '', cssTitleFont = 'text-dark'
    if (wnt.code.wtype && wnt.code.code) {
      title = (wnt.code.wtype=='c') ? '<Call> ' : '<Put> '
      title += wnt.code.code.toString() 
      if (wnt.code.wtype=='c')
        cssTitle = 'bg-purple', cssTitleFont = 'text-white'
      else
        cssTitle = 'bg-yellow', cssTitleFont = 'text-white'
    }
    // 7.0 正股標題
    var title2 = 'Stock '+this.props.no.toString()
    if (stock.code.code)
      var title2 = stock.code.code.toString()+' '+getUnderlyingName2(stock.code.code)
    
    // 8.0 即買&即賣
    var cssWntBuy = 'btn-secondary', isWntForceBuyDisable = isDisable
    if (wnt.buy.status == 'open') cssWntBuy = 'btn-success', isWntForceBuyDisable = false
    
    var cssWntSell = 'btn-secondary', isWntForceSellDisable = isDisable
    if (wnt.sell.status == 'open') cssWntSell = 'btn-success', isWntForceSellDisable = false
    
    // 9.0 pair按鈕
    var isPairDisable = isDisable
    if ((wnt.buy.status == 'open' && wnt.sell.status == 'open') || isDisable) isPairDisable = true
    
    // 10.0 正股 觸買
    var cssStockStatus1 = 'btn-secondary', cssStockStatus2 = 'btn-secondary', cssStockStatus3 = 'btn-secondary', cssStockStatus4 = 'btn-secondary'
    if (stock.status1 == 'open') cssStockStatus1 = 'btn-success'
    if (stock.status2 == 'open') cssStockStatus2 = 'btn-success'
    if (stock.status3 == 'open') cssStockStatus3 = 'btn-success'
    if (stock.status4 == 'open') cssStockStatus4 = 'btn-success'
    
    // 10.1 正股 觸買 输入框
    var isShowQty = false, isShowRatio = false
    if (!stock.status2 || stock.status2 == 'open')
      isShowQty = true
    else if (stock.status4 == 'open')
      isShowRatio = true
    
    // 11.0 操作
    var cssStockAction1 = 'btn-secondary', cssStockAction2 = 'btn-secondary', cssStockAction3 = 'btn-secondary', cssStockAction4 = 'btn-secondary',
        isStockAction1Disable = true, isStockAction2Disable = true, isStockAction3Disable = true, isStockAction4Disable = true
    
    // 11.1 is pair open?
    if (wnt.buy.status == 'open' || wnt.sell.status == 'open' || wnt.stopLoss.status == 'start')
      isStockAction1Disable = false
    
    // 11.2 action1
    if (stock.action1 == 'start') cssStockAction1 = 'btn-success'
    else if (stock.action1 == 'stop') cssStockAction1 = 'btn-secondary'
    else if (stock.action1 == 'fail') cssStockAction1 = 'btn-danger'
    
    if (stock.action1 == 'start' && (wnt.sell.status == 'open' || wnt.buy.status == 'open'))
      isStockAction2Disable = false
    
    if (stock.action1 == 'start' && wnt.stopLoss.status == 'start')
      isStockAction3Disable = false
    
    if (stock.action1 == 'start' && (wnt.buy.status == 'open' || wnt.sell.status == 'open'))
      isStockAction4Disable = false
    
    if (stock.action2 == 'start') cssStockAction2 = 'btn-success', isStockAction2Disable = false
    if (stock.action3 == 'start') cssStockAction3 = 'btn-success', isStockAction3Disable = false
    if (stock.action4 == 'start') cssStockAction4 = 'btn-success', isStockAction4Disable = true
    
    // 12.0 止蝕
    var isStopLossDisable = isDisable, isStopLossDisable2 = isDisable, cssStopLoss = 'btn-secondary'
    if (parseInt(wnt.position) > 0)
      isStopLossDisable = false
    
    if (wnt.stopLoss.status == 'start' && !stock.action2)
      cssStopLoss = 'btn-success'
    
    if (stock.action2 == 'start' || stock.action3 == 'start')
      isStopLossDisable2 = false

    // 13.0 track
    var css_nTrack = 'btn-secondary', css_aTrack = 'btn-secondary', css_xTrack = 'btn-secondary'
    if (stock.nTrack == 'start') css_nTrack = 'btn-success'
    if (stock.aTrack == 'start') css_aTrack = 'btn-success'
    if (stock.xTrack == 'start') css_xTrack = 'btn-success'
    
    // 14.0 layout
    var isLayout1Disable = false, isLayout2Disable = false
    if (config.layout == 'normal') isLayout2Disable = true
    else if (config.layout == 'xlarge') isLayout1Disable = true

    return(
      <React.Fragment>

<div className="col-12 col-sm-6 col-md-3 mb-2 mb-sm-3 drag-cell">
<div className={classNames("cell", config.layout)}>

  <div className="row">
  <div className="col-12 col-sm-12 mb-2 mb-sm-2">
  <div className={classNames("header d-flex justify-content-between", cssTitle)}>
    <span className={classNames('text-right', cssTitleFont)}> {title} </span> 
    <span className="text-left"></span>
  </div>
  </div>
  </div>

  <div className="row">
  <div className="col-4 col-sm-4 pr-2 pr-sm-2">
    <table className="table price-table">
    <thead>
      <tr><th colSpan="2"><input type="text" name="wnt" className={classNames('form-control', cssWnt)} value={wnt.code.code || ''} onChange={this.handleChange} disabled={isWntDisable} /></th></tr>
    </thead>
    <tbody>
      <tr><td> {wnt.priceTable.ask.price} </td><td> {wnt.priceTable.bid.price} </td></tr>
      <tr><td> {wnt.priceTable.ask.qty} </td><td> {wnt.priceTable.bid.qty} </td></tr>
      {isShowWntPriceTable &&
      <tr><td colSpan="2">
        <div className="d-flex">
        <span style={{width: wnt.priceTable.ask.ratio+"%"}} className="alert-success text-left text-success">&nbsp;</span>
        <span style={{width: wnt.priceTable.bid.ratio+"%"}} className="alert-danger text-right text-danger">&nbsp;</span>
        </div>
        <div className="d-block bar">
        <span className="float-left text-success"> {wnt.priceTable.ask.ratio}% </span>
        <span className="float-right text-danger"> {wnt.priceTable.bid.ratio}% </span>
        </div>
      </td></tr>}
    </tbody>
    </table>
  </div>
  
  <div className="col-8 col-sm-8 pl-0 pl-sm-0 pr-0 pr-sm-0 mb-3 mb-sm-3">
    <div className="col-12 col-sm-12 pl-0 pl-sm-0 pr-0 pr-sm-0 d-flex">
      <InputGroupTicket
        key={"wnt.buy.price"}
        no={this.props.no}
        data1={wnt.buy.price}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      <InputGroupTicket
        key={"wnt.buy.qty"}
        no={this.props.no}
        data1={wnt.buy.qty}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      <button className={classNames("btn btn-sm", cssWntBuy)} type="button" name="wntBuy" onClick={this.handleClick} disabled={isWntForceBuyDisable}>Buy</button>
    </div>
    
    <div className="col-12 col-sm-12 pl-0 pl-sm-0 pr-0 pr-sm-0 d-flex">
      <InputGroupTicket
        key={"wnt.sell.price"}
        no={this.props.no}
        data1={wnt.sell.price}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      <InputGroupTicket
        key={"wnt.sell.qty"}
        no={this.props.no}
        data1={wnt.position}
        data2={true || isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      <button className={classNames("btn btn-sm", cssWntSell)} type="button" name="wntSell" onClick={this.handleClick} disabled={isWntForceSellDisable}>Sell</button>
    </div>
    
    <div className="col-12 col-sm-12 pl-0 pl-sm-0 pr-0 pr-sm-0 d-flex">
      <InputGroupTicket
        key={"wnt.stopLoss.price"}
        no={this.props.no}
        data1={wnt.stopLoss.price}
        data2={isStopLossDisable2}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      <input type="text" value="" onChange={this.handleChange} disabled={true} />
      <button className={classNames("btn btn-sm", cssStopLoss)} name="stoploss" type="button" onClick={this.handleClick} disabled={isStopLossDisable}>StopLoss</button>
    </div>
    
    <div className="col-12 col-sm-12 pl-0 pl-sm-0 pr-0 pr-sm-0">
      <button className="btn btn-sm btn-secondary d-none" type="button" disabled={true || isDisable}>Copy</button>
      <button className="btn btn-sm btn-secondary" name="pair" type="button" onClick={this.handleClick} disabled={isPairDisable}>Pair B&S </button>
    </div>
  </div>
  </div>

  <div className="row">
  <div className="col-12 col-sm-12">
    <textarea rows="2" value={htmlMsg} disabled></textarea>
  </div>
  </div>

  <div className="row">
  <div className="col-12 col-sm-12 mt-2 mt-sm-2 mb-2 mb-sm-2">
    <div className="header">
      <button className={classNames("btn btn-sm", cssStockStatus1)} name="stock.status1" type="button" onClick={this.handleClick}  disabled={true || isDisable}>UI</button>
      <span className="ml-1 ml-sm-1 mr-1 mr-sm-1"> {title2} </span>
      <button className={classNames("btn btn-sm", cssStockStatus2)} name="stock.status2" type="button" onClick={this.handleClick}  disabled={isDisable}>Q</button>
      <button className={classNames("btn btn-sm", cssStockStatus3)} name="stock.status3" type="button" onClick={this.handleClick}  disabled={true || isDisable}>R</button>
      <button className={classNames("btn btn-sm", cssStockStatus4)} name="stock.status4" type="button" onClick={this.handleClick}  disabled={isDisable}>RT</button>
    </div>
  </div>
  </div>
  
  <div className="row">
  <div className="col-4 col-sm-4 pr-2 pr-sm-2">
    <table className="table price-table">
    <thead>
      <tr><th colSpan="2"><input type="text" className="form-control" value={stock.code.code || ''} onChange={this.handleChange} disabled={true} /></th></tr>
    </thead>
    <tbody>
      <tr><td> {stock.priceTable.ask.price} </td><td> {stock.priceTable.bid.price} </td></tr>
      <tr><td> {stock.priceTable.ask.qty} </td><td> {stock.priceTable.bid.qty} </td></tr>
      {isShowStockPriceTable &&
      <tr><td colSpan="2">
        <div className="d-flex">
        <span style={{width: stock.priceTable.ask.ratio+"%"}} className="alert-success text-left text-success">&nbsp;</span>
        <span style={{width: stock.priceTable.bid.ratio+"%"}} className="alert-danger text-right text-danger">&nbsp;</span>
        </div>
        <div className="d-block bar">
        <span className="float-left text-success"> {stock.priceTable.ask.ratio}% </span>
        <span className="float-right text-danger"> {stock.priceTable.bid.ratio}% </span>
        </div>
      </td></tr>}
    </tbody>
    </table>
  </div>
  
  <div className="col-5 col-sm-5 pl-0 pl-sm-0 pr-0 pr-sm-0">
    <div className="mb-1 mb-sm-1">
      <button className={classNames("btn btn-sm", css_nTrack)} name="nTrack" type="button" onClick={this.handleClick} disabled={isDisable}>nTrack</button>
      <button className={classNames("btn btn-sm", css_aTrack)} name="aTrack" type="button" onClick={this.handleClick} disabled={isDisable}>aTrack</button>
    </div>
    
    <div className="mb-1 mb-sm-1">
      <button className={classNames("btn btn-sm", css_xTrack)} name="xTrack" type="button" onClick={this.handleClick} disabled={isDisable}>xTrack</button>
      <button className="btn btn-sm btn-secondary" name="Tick" type="button" onClick={this.handleClick} disabled={true || isDisable}>Tick</button>
    </div>
    
    <div className="d-flex d-block">
      <InputGroupTicket
        key={"stock.buy.price"}
        no={this.props.no}
        data1={stock.buy.price}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      
      {isShowQty &&
      <InputGroupTicket
        key={"stock.buy.qty"}
        no={this.props.no}
        data1={stock.buy.qty}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />}
      
      {isShowRatio &&
      <InputGroupTicket
        key={"stock.buy.ratio"}
        no={this.props.no}
        data1={stock.buy.ratio}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />}
    </div>
    
    <div className="d-flex d-block">
      <InputGroupTicket
        key={"stock.sell.price"}
        no={this.props.no}
        data1={stock.sell.price}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />
      
      {isShowQty &&
      <InputGroupTicket
        key={"stock.sell.qty"}
        no={this.props.no}
        data1={stock.sell.qty}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />}
      
      {isShowRatio &&
      <InputGroupTicket
        key={"stock.sell.ratio"}
        no={this.props.no}
        data1={stock.sell.ratio}
        data2={isDisable}
        lang={this.props.lang}
        initInputData={this.initInputData}
        setStates={this.props.setStates}
        getStates={this.props.getStates}
      />}
    </div>
    
  </div>
  
  <div className="col-3 col-sm-3 pl-2 pl-sm-2 pr-0 pr-sm-0">
    <button className={classNames("btn btn-sm mb-1 mb-sm-1", cssStockAction1)} name="stock.action1" type="button" onClick={this.handleClick} disabled={isStockAction1Disable}> {capitalize(this.props.data.stock.action1) || 'Start'} </button> <br />
    <button className={classNames("btn btn-sm mb-1 mb-sm-1", cssStockAction2)} name="stock.action2" type="button" onClick={this.handleClick} disabled={isStockAction2Disable}> {capitalize(this.props.data.stock.action2) || 'Amend'} </button> <br />
    <button className={classNames("btn btn-sm mb-1 mb-sm-1", cssStockAction3)} name="stock.action3" type="button" onClick={this.handleClick} disabled={isStockAction3Disable}> {capitalize(this.props.data.stock.action3) || 'Mon&Amend'} </button> <br />
    <button className={classNames("btn btn-sm mb-1 mb-sm-1", cssStockAction4)} name="stock.action4" type="button" onClick={this.handleClick} disabled={isStockAction4Disable}>Force</button> <br />
  </div>
  </div>
  
  
  <div className="row">
  <div className="col-12 col-sm-12">
  
  <div className="d-flex d-block float-left">
    <button type="button" name="updatePrice1" className="btn btn-secondary" onClick={this.handleClick} disabled={isDisable}> Update </button>
    {false && <div className="form-check">
      <input className="form-check-input" type="checkbox" name="updatePrice2" value={wnt.code.code || ''} id="updatePrice2" checked={this.props.data.updatePrice.isUpdate2} onChange={this.handleChange} disabled={isDisable}></input>
      <label className="form-check-label" className="updatePrice2">3s</label>
    </div>}
  </div>
  
  <div className="float-right">
    <div className="btn-group">
    <button type="button" name="subtract" className="btn btn-secondary" disabled={isLayout2Disable} onClick={this.handleLayout}> - </button>
    <button type="button" name="add" className="btn btn-secondary" disabled={isLayout1Disable} onClick={this.handleLayout}> + </button>
    </div>
  </div>
  
  </div>
  </div>

</div>
</div>

      </React.Fragment>
    )
  }
}