class FullAuto extends React.Component {
  constructor(props) {
    super(props)
    this.setStates = this.setStates.bind(this)
    this.getStates = this.getStates.bind(this)
    
    this.state = {}
    
    this.state.config = {
      defaultValue: ['bull', 'bear'],
      value: this.props.config
    }
    
    this.state.modules = {bull: null, bear: null, call: null, put: null}
    this.state.prefix = 'u000_'
    this.state.userId = parseInt(Cookies.get(this.props.userId))
    this.state.underlyingDefault = this.props.underlyingDefault
    
    this.state.recovery = {}
    this.state.recovery.isRecoveryEnd = false
    
    this.state.sizeReceiptData = {totalBytes: 0, noPackage: 0, lastAliveTime: null}
    
    this.state.issuer = {curState: null, curIssuer: null, feedback: null, responseResult: null, selected: []}
    this.state.marketStatus = null
    this.state.betSize = null
    this.state.underlying = {curState: null, curUnderlying: null, feedback: null, responseResult: null, selected: [], removed: []}
    this.state.underlyingList = null
    this.state.wntPrice = {}
    this.state.wntList = {}
    this.state.wntList2 = {}
    this.state.wntType = {}
    this.state.stockPrice = {}
    
    var cells = []
    var orders = []
    var positions = []
    var portfolios = []
    
    // 生成数据集
    for(var i=0; i<4; i++) {
      cells[i] = {}
      
      cells[i].action = {
        code: {value: '', feedback: '', valid: 'number'},
        symbol: {value: '', feedback: '', valid: ''}, 
        issuerSize: {value: '', feedback: '', valid: '', responseResult: ''}, 
        quantity: {value: '', feedback: '', valid: '', responseResult: ''},
        spread: {value: '', feedback: '', valid: 'number'},
        status: {isSet: false, isStart: false, isPause: false, isStop: false, needToSetAgain: false, result: ''}
      }
      
      cells[i].priceTable = {}
      
      cells[i].wPrice = {bid: null, ask: null}
      
      cells[i].setting = {
        inout: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        wintick: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        inlvl: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        stoplost: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        ptrange: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
      }
      
      cells[i].trade = {
        price: {value: '', feedback: '', valid: 'number_except_zero'},
        size: {value: '', feedback: '', valid: 'string'}
      }
      
      orders[i] = []
      positions[i] = []
      portfolios[i] = []
    }
    
    this.state.cells = cells
    this.state.orders = orders
    this.state.positions = positions
    this.state.positionActions = {}
    this.state.portfolios = portfolios
    this.state.codeMapping = {}
    this.state.codeId = []
    this.state.signal = {}
    this.msg = []
    
    this.sendUWarrantlist = this.sendUWarrantlist.bind(this)
  }
  
  componentDidMount() {
    const render = (res) => {
      // 記錄狀態
      this.state.sizeReceiptData.totalBytes += res.length*2
      this.state.sizeReceiptData.noPackage += 1
      this.state.sizeReceiptData.lastAliveTime = Date.now()
      
      var data = JSON.parse(res)
      console.log(data)
      var obj = $.extend(true, {}, this.state)
      
      // 接口v3
      if (!res.includes('connect_alive'))
        this.msg.push(data)
      if ('action' in data) {
        // 接口v1
        if (data.action=='pricetable') {obj = this.setPriceTable(obj, data)}
        else if(data.action=='wprice') {obj = this.setWprice(obj, data)}
        else if (data.action=='import') {}
        else if (data.action=='bind_top') {}
        else if (data.action=='load') {}
        else if (data.action=='algo') {obj = this.setAlgo(obj, data)}
        else if (data.action=='command set pair') {obj = this.setPairs(obj, data)}
        else if (data.action=='command start') {obj = this.setStart(obj, data)}
        else if (data.action=='command stop') {obj = this.setStop(obj, data)}
        else if (data.action=='command pause') {obj = this.setPause(obj, data)}
        else if (data.action=='on_order') {obj = this.setPosition(obj, data)}
        else if (data.action=='BUY') {obj = this.setOnOrder(obj, data)}
        else if (data.action=='SELL') {obj = this.setOnOrder(obj, data)}
        else if (data.action=='portfolio' && ('mode' in data)) {obj = this.setPorfololio(obj, data)}
        else if (data.action=='command param') {obj = this.setParam(obj, data)}
        else if (data.action=='set_position') {obj = this.checkPosition(obj, data)}
        else if (data.action=='previous order') {obj = this.resetPosition(obj, data)}
        else if (data.action=='on_command') {obj = this.checkOnCommand(obj, data)}
        // 接口v2
        else if (data.action=='order') {obj = this.setOnOrder(obj, data)}
        else if (data.action=='delete order') {obj = this.resetPosition(obj, data)}
        else if (data.action=='portfolio' && ('algo_name' in data)) {obj = this.setPorfololioV2(obj, data)}
        else if (data.action=='cmd set') {obj = this.checkOnCommand(obj, data)}
        else if (data.action=='connect_alive') { }
        else if (data.action=='connect_reject') { this.connectReject(data)}
        // 接口v3
        else if (data.action=='selectissuer') {this.setIssuer(obj, data)}
        else if (data.action=='issuerlist') {this.setIssuerList(obj, data)}
        else if (data.action=='marketstatus') {this.setMarketstatus(obj, data)}
        else if (data.action=='betsize') {this.setBetSize(obj, data)}
        else if (data.action=='underlyinglist') {this.setUnderlyingList(obj, data)}
        else if (data.action=='selectunderlying') {this.setUnderlying(obj, data)}
        else if (data.action=='signal') {this.setSignal(obj, data)}
        else if (data.action=='stoplost') {this.setStoplost(obj, data)}
        else if (data.action=='winlvlsell' || data.action== 'winsell') {this.setPositionAction(obj, data)}
        else if (data.action=='uwarrantlist') {obj = this.setUWarrantList(obj, data)}
        else if (data.action=='selectwarrant') {obj = this.setSelectUWarrant(obj, data)}
      }
      // 接口v2
      else if ('msg_type' in data) {
        if (data.msg_type=='pricetable') {obj = this.setPriceTable(obj, data)}
        else if (data.msg_type=='algo_set_msg') {obj = this.setPairsV2(obj, data)}
        else if (data.msg_type=='wprice') {obj = this.setWprice(obj, data)}
        else if (data.msg_type=='bear_algo_set_msg') {obj = this.setPairsV2(obj, data)}
        else if (data.msg_type=='algo_param_msg') {obj = this.setParam(obj, data)}
        else if (data.msg_type=='cbbc_algo_force_buy') {obj = this.checkForce(obj, data)}
        else if (data.msg_type=='cbbc_algo_force_sell') {obj = this.checkForce(obj, data)}
      }
      // 接口v3
      else if ('algo_name' in data && data.algo_name.includes('s1')) {
        if(data.action=='start') {}
        else if (data.action=='pause') {}
      }
      // 接口v2
      else if ('action_type' in data) {
        if (data.action_type=='start') {obj = this.setStart(obj, data)}
        else if (data.action_type=='pause') {obj = this.setPause(obj, data)}
        else if (data.action_type=='stop') {obj = this.setStop(obj, data)}
      }
      // 接口v2
      else if ('cmd' in data) {
        if (data.cmd=='get_algo_names') {obj = this.setAlgoV2(this, obj, data)}
      }
      else if ('type' in data) {
        // 接口v2
        if (data.type=='set position') {obj = this.checkPositionV2(obj, data)}
        // 公共
        else if (data.type=='recovery_end') {obj = this.setRecoveryEnd(obj, data)}
      }
      this.setState(obj)
    }
    
    this.render2 = render
    initWebsocket(render)
    initTablePrice()
  }
  
  // 盘口 set pairs v1
  setPairs(state, pair) {
    var id = pair.ref.replace(state.prefix, '')
    // 正确 code
    if (pair.warrant_code && pair.symbol && pair.result.toLowerCase()=='success') {
      state.cells[id].action.code.value = pair.warrant_code.toString()
      state.cells[id].action.symbol.value = pair.symbol.toString()
      state.cells[id].action.issuerSize.value = formatInputUnit(pair.issuer_size, false).toString()
      state.cells[id].action.quantity.value = formatInputUnit(formatLong(pair.buy_quantity), false).toString()
      state.cells[id].action.spread.value = formatInputUnit(formatLong(pair.spread), false).toString()
      state.cells[id].action.status.isSet = true
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.result = ''
    }
    // 错误 code
    else if (pair.warrant_code && pair.symbol && pair.result.toLowerCase()=='fail') {
      // 错误信息
      state.cells[id].action.code.feedback = pair.reason
      // 重设状态
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isSet = false
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.needToSetAgain = false
    }
    return {cells: state.cells}
  }
  
  
  // 盘口 set pairs v2
  setPairsV2(state, pair) {
    var id = pair.ref.replace(state.prefix, '')
    // 正确 code
    if (('pair' in pair) &&
        ('warrant_code' in pair.pair) && pair.pair.warrant_code && 
        ('symbol' in pair.pair) && pair.pair.symbol && 
        ('result' in pair) && pair.result.toLowerCase()=='success') {
      state.cells[id].action.code.value = pair.pair.warrant_code.toString()
      state.cells[id].action.symbol.value = pair.pair.symbol.toString()
      state.cells[id].action.issuerSize.value = formatInputUnit(pair.pair.issuer_size, false).toString()
      state.cells[id].action.quantity.value = formatInputUnit(formatLong(pair.pair.buy_quantity), false).toString()
      state.cells[id].action.spread.value = formatInputUnit(formatLong(pair.pair.spread), false).toString()
      state.cells[id].action.status.isSet = true
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.result = ''
    }
    // 错误 code
    else if (('reason' in pair) && pair.reason.length>=0 && 
             ('result' in pair) && pair.result.toLowerCase()=='fail') {
      // 错误信息
      state.cells[id].action.code.feedback = pair.reason
      // 重设状态
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isSet = false
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.needToSetAgain = false
    }
    return {cells: state.cells}
  }
  
  // 盘口 start
  setStart(state, data) {
    var id = data.ref.replace(state.prefix, '')
    if (data.result.toLowerCase() == 'success') {
      state.cells[id].action.status.isSet = false
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isStart = true
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.result = ''
    }
    else if (data.result.toLowerCase() == 'fail') {
      state.cells[id].action.status.result = data.reason
    }
    return {cells: state.cells}
  }
  
  // 盘口 stop
  setStop(state, data) {
    var id = data.ref.replace(state.prefix, '')
    if (data.result.toLowerCase() == 'success') {
      state.cells[id].action.status.isSet = false
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = true
      state.cells[id].action.status.result = ''
    }
    else if (data.result.toLowerCase() == 'fail') {
      state.cells[id].action.status.result = data.reason
    }
    return {cells: state.cells}
  }
  
  // 盘口 pause
  setPause(state, data) {
    var id = data.ref.replace(state.prefix, '')
    if (data.result.toLowerCase() == 'success') {
      state.cells[id].action.status.isSet = false
      state.cells[id].action.status.isPause = true
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.result = ''
    }
    else if (data.result.toLowerCase() == 'fail') {
      state.cells[id].action.status.result = data.reason
    }
    return {cells: state.cells}
  }
  
  // Algo v1
  setAlgo(state, data) {
    for (var algo of data.data)
      if (algo.includes('cbbc'))
        state.modules.bull = algo
      else if (algo.includes('bear'))
        state.modules.bear = algo
    // 報價表
    for (var i in state.config.value) {
      var code = state.cells[i].action.code.value
      // 已输入编号, 才要报价表
      if (code.length > 0) {
        var command = ['loadpricetable', code, state.prefix+i]
        var command1 = {type: "algo_command", "key": state.modules[state.config.value[i]], command: command.join('|')}
        sendWebsocket(JSON.stringify(command1))
      }
    }
    
    return {modules: state.modules}
  }
  
  // Algo v2
  setAlgoV2(_this, state, data) {
    for (var algo in data.algos)
      if (algo.includes('bear'))
        state.modules.bull = algo, state.modules.bear = algo
      else if (algo.includes('s1'))
        state.modules.call = algo, state.modules.put = algo
      else if (algo.includes('csalgo'))
        state.modules.call = algo, state.modules.put = algo
      else
        state.modules.bull = algo, state.modules.bear = algo, state.modules.call = algo, state.modules.put = algo
    // 沒有cbbc algo
    if ( (state.modules.bull != null && state.modules.bull.includes('bear')) && (state.modules.bear != null && state.modules.bear.includes('bear')) )
      console.log({log: 'algo exist'})
    // 报价表
    async function loadPrice(state) {
      for (var i in state.config.value) {
        var code = state.cells[i].action.code.value
        var wtype = state.config.value[i]
        // 已输入编号, 才要报价表
        if (code.length > 0) {
          // 等待2秒, 避免过载
          function wait(ms) {
            var start = new Date().getTime(), end = start
            while(end < start + ms)
              end = new Date().getTime()
          }
          await wait(2000)
          // 传送
          var algo = state.modules[wtype]
          if (algo) {
            var command = {
              cmd: "loadpricetable",
              action: "loadpricetable",
              warrant_code: parseInt(code),
              ref: state.prefix+i,
              id: parseInt(state.userId),
              algo_name: algo
            }
            sendWebsocket(JSON.stringify(command))
          }
          else if (!algo)
            console.log('Algo not found. Cannot load price table.')
        }
      }
    }
    loadPrice(state)
    
    // 發行人
    var userId = parseInt(state.userId)
    var issuer = state.issuer.curIssuer
    var algoName = (state.modules.call) ? state.modules.call : state.modules.put
    var command = {cmd: 'issuerlist', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString()}
    sendWebsocket(JSON.stringify(command))
    
    // 
    var command2 = {cmd: 'marketstatus', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), action: ''}
    sendWebsocket(JSON.stringify(command2))
    
    //
    var command3 = {cmd: 'underlyinglist', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString()}
    sendWebsocket(JSON.stringify(command3))
    
    //
    _this.testData(_this.render2)
    
    return {modules: state.modules}
  }
  
  // 报价表
  setPriceTable(state, data) {
    // 数据源
    var code=data.warrant_code,
        wkey=parseFloat(formatLong(data.wkey)).toFixed(3),
        fprice=(formatPrice(data.fprice)),
        side=data.side.toLowerCase(),
        no=data.ref.replace(state.prefix, '')
    // 插值
    if (!state.cells[no].priceTable.hasOwnProperty(wkey))
      state.cells[no].priceTable[wkey] = {bid: null, ask: null}
    state.cells[no].priceTable[wkey][side] = fprice
    // 排序
    var temp = {};
    Object.keys(state.cells[no].priceTable).sort().forEach(function(k) {
      temp[k] = state.cells[no].priceTable[k];
    });
    state.cells[no].priceTable = temp
    return {cells: state.cells}
  }
  
  // 明細 v1
  setPorfololio(state, data) {
    var id = data.ref.replace(state.prefix, '')
    var arr = {
      ref:data.ref, mode:data.mode, key: data.key,
      code: data.warrant_code, quantity: formatLong(data.quantity),
      buyPrice: formatLong(data.buyprice), buyTime: formatDate(data.buytime),
      sellPrice: formatLong(data.sellprice), soldTime: formatDate(data.soldtime),
      profitLoss: (formatLong(data.sellprice)-formatLong(data.buyprice))*formatLong(data.quantity)
    }
    state.portfolios[id].push(arr)
    return {portfolios: state.portfolios}
  }
  
  // 明細 v2
  setPorfololioV2(state, data) {
    // 數據映射, 輪證<=>正股
    if (!(data.ref in state.codeMapping) && ('ucode' in data) && data.ucode>0)
      state.codeMapping[data.ref] = formatCode(data.ucode, 4)
    if (!state.codeId.includes(data.ref))
      state.codeId.push(data.ref)
    
    // 數據映射, 輪證<=>codeMapping id
    var id = (data.ref.replace(/[^0-9]/g, '').length == 5)
      ? state.codeId.indexOf(data.ref)        // a1
      : data.ref.replace(state.prefix, '')    // cbbc
    
    var arr = {
      ref:data.ref, mode:data.id,
      key: data.algo_name,
      code: data.warrant_code,
      ucode: (data.ucode) ? formatCode(data.ucode, 4) : '',
      quantity: formatLong(data.quantity),
      buyPrice: formatLong(data.buy_price), buyTime: formatDate(data.buytime),
      sellPrice: formatLong(data.sell_price), soldTime: formatDate(data.sellime),
      profitLoss: (formatLong(data.sell_price)-formatLong(data.buy_price))*formatLong(data.quantity),
      issuer: data.issuer
    }
    if(!(id in state.portfolios))
      state.portfolios[id] = []
    state.portfolios[id].push(arr)
    return {portfolios: state.portfolios}
  }
  
  // 買賣
  setOnOrder(state, data) {
    // 數據映射, 輪證<=>正股
    if (!(data.ref in state.codeMapping) && ('ucode' in data) && data.ucode>0)
      state.codeMapping[data.ref] = formatCode(data.ucode, 4)
    if (!state.codeId.includes(data.ref))
      state.codeId.push(data.ref)
    
    // 數據映射, 輪證<=>codeMapping id
    var id = (data.ref.replace(/[^0-9]/g, '').length == 5)
      ? state.codeId.indexOf(data.ref)        // a1
      : data.ref.replace(state.prefix, '')    // cbbc
    
    var arr = {
      code: data.warrant_code, 
      ucode: (data.ucode) ? formatCode(data.ucode, 4) : '',
      // side: data.action.toLowerCase(),
      side: data.side.toLowerCase(),
      status: data.status,
      transactionTm: ((data.transaction_time) ? formatDate(data.transaction_time) : getCurDateTime()),
      matchPrice: formatLong(data.filled_price), 
      matchQuantity: formatLong(data.filled_quantity),
      totalPrice: formatLong(data.filled_price)*formatLong(data.filled_quantity),
      futurePrice: ('sellout' in data) ? formatPrice(data.sellout) : ('buyin' in data) ? formatPrice(data.buyin) : '',
      stoplost: (data.stoplost) ? formatLong(data.stoplost) : '',
      reason: ('reason' in data) ? data.reason: '',
      wbid: formatLong(data.wbid)
    }
    
    if (!(id in state.orders))
      state.orders[id] = []
    if (!(id in state.positions))
      state.positions[id] = []
    
    // 真實買賣
    if (data.transaction_time)
      state.orders[id].push(arr)
    // 真實買賣+借仓(用戶另外set prsitions)
    state.positions[id].push(arr)
    
    // 删除旧的wprice
    if (data.warrant_code in state.wntPrice)
      delete state.wntPrice[data.warrant_code]
    return state
  }
  
  //
  setPositionAction(state, data) {
    if (!(data.code in state.positionActions))
      state.positionActions[data.code] = {}
    
    state.positionActions[data.code][data.action] = {
      result: data.result.toLowerCase(),
      setaction: data.setaction.toLowerCase()
    }
    return {positionActions: state.positionActions}
  }
  
  // 倉位 (已经没有这个功能)
  setPosition(state, data) {
    /*var id = data.ref.replace(state.prefix, '')
    var arr = {
      code: data.order.code,
      side: data.order.side.replace('order_side::', '').toLowerCase(), 
      status: data.order.status.replace('order_status::', ''), 
      transactionTm: formatDate(data.order.transaction_tm),
      matchPrice: formatLong(data.order.match_price), 
      matchQuantity: data.order.match_quantity, 
      totalPrice: formatLong(data.order.match_price*data.order.match_quantity)
    }
    state.positions[id].push(arr)*/
    return {positions: state.positions}
  }
  
  // 初始化
  setRecoveryEnd(state, data) {
    state.recovery.isRecoveryEnd = true
    return {recovery: state.recovery}
  }
  
  // 標的掛牌價
  setWprice(state, data) {
    var id = data.ref.replace(state.prefix, ''), side = data.side.toLowerCase()
    if ((id in state.cells) && ('wPrice' in state.cells[id])) {
      state.cells[id].wPrice[side] = formatLong(data.wprice)
      return {cells: state.cells}
    }
    
    if (('price' in data) && ('side' in data) && ('ref' in data)) {
      if (!(data.ref in state.wntPrice)) state.wntPrice[data.ref] = {bid: null, ask: ''}
      state.wntPrice[data.ref][side] = formatLong(data.price)
      return {wntPrice: state.wntPrice}
    }
  }
  
  // 参数
  setParam(state, data) {
    var id = data.ref.replace(state.prefix, ''), type = data.type.toLowerCase(), result = data.result.toLowerCase()
    
    // setting
    if (type in state.cells[id].setting)
      if (result=='success')
        state.cells[id].setting[type].responseResult = result, state.cells[id].setting[type].value = data.value
      else
        state.cells[id].setting[type].feedback = result
    
    // action
    var keys1 = {isize: 'issuerSize', qty: 'quantity'}
    if (type in keys1) {
      if (result=='success'){
        // 添加样式
        state.cells[id].action[keys1[type]].responseResult = result
        // 注入val
        if (keys1[type]=='issuerSize')
          var val = data.value
        else if (keys1[type]=='quantity' || true)
          var val = formatLong(data.value)
        state.cells[id].action[keys1[type]].value = formatInputUnit(val, false).toString()
      }
      else if (result=='fail'){
        state.cells[id].action[keys1[type]].feedback = result
      }
    }
    return {cells: state.cells}
  }
  
  // 检验size v1
  checkPosition(state, data) {
    var id = data.ref.replace(state.prefix, ''), result = data.result.toLowerCase()
    if (result=='success') {}
    else if (result=='fail')
      state.cells[id].trade.size.feedback = data.reason
    return {cells: state.cells}
  }
  
  // 检验size v2
  checkPositionV2(state, data) {
    var id = data.ref.replace(state.prefix, ''), result = data.result.toLowerCase()
    if (result=='success') {}
    else if (result=='invalid status')
      state.cells[id].trade.size.feedback = data.result
    return {cells: state.cells}
  }
  
  // 設置手持position
  resetPosition(state, data) {
    var id = data.ref.replace(state.prefix, '')
    // 重設position=0
    if (data.filled_price==0 && data.order_quantity==0)
      var side = 'reset'
    // 設置有其他position
    else if (data.filled_price>0 && data.order_quantity>0)
      var side = 'buy'
    else
      var side = ''
    
    var arr = {
      code: data.warrant_code,
      futurePrice: null,
      matchPrice: formatLong(data.filled_price),
      matchQuantity: formatLong(data.order_quantity),
      reason: null,
      side: side,
      status: data.status,
      totalPrice: formatLong(data.filled_price)*formatLong(data.order_quantity),
      transactionTm: ((data.transaction_time) ? formatDate(data.transaction_time) : getCurDateTime())
    }
    state.positions[id].push(arr)
    return {positions: state.positions}
  }
  
  // 错 Code & Symbol
  checkOnCommand(state, data) {
    var id = data.ref.replace(state.prefix, '')
    if (data.result.toLowerCase()=='fail') {
      if (data.reason == 'fail command set underlying code omdd mapping not found')
        state.cells[id].action.symbol.feedback = 'Invalid Symbol'
      else if (data.reason == 'Invalid warrant Code')
        state.cells[id].action.code.feedback = 'Invalid Code'
    }
    return {cells: state.cells}
  }
  
  // 即時買入 & 即時賣出
  checkForce(state, data) {
    var id = data.ref.replace(state.prefix, '')
    if(data.result.toLowerCase()=='fail') {
      if(data.msg_type=='cbbc_algo_force_buy')
        state.cells[id].trade.size.feedback = data.reason
      else if (data.msg_type=='cbbc_algo_force_sell')
        state.cells[id].trade.size.feedback = data.reason
    }
    return {cells: state.cells}
  }
  
  // 強制登出
  connectReject(data) {
    alert(data.error);
    global.func.logout();
  }
  
  // 发行人
  setIssuer(state, data) {
    if (state.issuer.curIssuer) {
      if ('selectaction' in data)
        state.issuer.curState = data.selectaction
      if('result' in data && data.result.toLowerCase()=='fail') {
        state.issuer.feedback = data.reason
        state.issuer.responseResult = null
      }
      else if ('result' in data && data.result.toLowerCase()=='success') {
        state.issuer.feedback = null
        state.issuer.responseResult = 'success'
        if(data.selectaction=='select')
          state.issuer.selected.push(data.issuer)
        else if (data.selectaction=='remove')
          state.issuer.selected.splice(state.issuer.selected.indexOf(data.issuer), 1)
      }
    }
    return {issuer: state.issuer}
  }
  
  // 发行人
  setIssuerList(state, data) {
    if (data.issuers)
      state.issuer.selected = data.issuers.split(",")
    if (!state.issuer.curIssuer && state.issuer.selected.length)
      state.issuer.curIssuer = state.issuer.selected[0]
    return {issuer: state.issuer}
  }
  
  //
  setMarketstatus(state, data) {
    if(data.new_action_status)
      state.marketStatus = data.new_action_status
    return {marketStatus: state.marketStatus}
  }
  
  //
  setBetSize(state, data) {
    if('betsize' in data)
      state.betSize = data.betsize
    return {betSize: state.betSize}
  }
  
  //
  setUnderlyingList(state, data) {
    if ('ucodes' in data) {
      state.underlyingList = []
      for (var ucode of data.ucodes.split(","))
        state.underlyingList.push(formatCode(ucode, 5))
      state.underlyingList.sort()
    }
    return {underlyingList: state.underlyingList}
  }
  
  //
  setUnderlying(state, data) {
    if(state.underlying.curUnderlying) {
      if ('selectaction' in data)
        state.underlying.curState = data.selectaction
      if('result' in data && data.result.toLowerCase()=='fail') {
        state.underlying.feedback = data.reason
        state.underlying.responseResult = null
      }
      else if ('result' in data && data.result.toLowerCase()=='success') {
        state.underlying.feedback = null
        state.underlying.responseResult = 'success'
      }
    }
    
    if ('result' in data && 'selectaction' in data && data.result.toLowerCase()=='success') {
      var ucode = formatCode(data.ucode, 5)
      if (data.selectaction == 'select') {
        // 預設1 default select
        state.underlying.selected.push(ucode)
        // 預設2 default remove
        var i = state.underlying.removed.indexOf(ucode)
        if (i>=0)
          state.underlying.removed.splice(i, 1)
      }
      else if (data.selectaction == 'remove') {
        // 預設1 default select
        var i = state.underlying.selected.indexOf(ucode)
        if (i>=0)
          state.underlying.selected.splice(i, 1)
        // 預設2 default remove
        state.underlying.removed.push(ucode)
      }
    }
    return {underlying: state.underlying}
  }
  
  //
  setSignal(state, data) {
    if (('ref' in data) && ('detected_ask' in data) && ('detected' in data)) {
      if (data.detected === true) {
        //
        if (!(data.ref in state.signal)) state.signal[data.ref] = {}
        //
        state.signal[data.ref].ask = formatLong(data.detected_ask)
        var detectedlist = []
        //
        for (var v of data.detectedlist) {
          var temp = v.warrantdesc.split("|")
          for (var k in temp) 
            temp[k] = temp[k].replace(/\s+/, "") 
          //
          if (temp.length>=2)
            temp[2] = formatLong(temp[2])
          detectedlist.push(temp)
        }
        state.signal[data.ref].detectedlist = detectedlist
      }
      //
      else if (data.detected === false) {
        if (data.ref in state.signal)
          delete state.signal[data.ref]
      }
    }
    return {signal: state.signal}
  }
  
  //
  setStoplost(state, data) {
    state.stockPrice[data.code] = {
      stoplost: formatLong(data.stoplost),
      wbid: formatLong(data.wbid)
    }
    return {stockPrice: state.stockPrice}
  }
  
  setUWarrantList(state, data) {
    state.wntList.ucode = data.ucode
    state.wntList.issuer = data.issuer
    state.wntList.codes = {}
    if (data.codes) {
      var codes = data.codes.split(",")
      for (var v of codes) {
        var data2 = v.split(":")
        var code = data2[0], status = data2[1]
        state.wntList.codes[code] = status
      }
    }
    return {wntList: state.wntList}
  }
  
  setSelectUWarrant(state, data) {
    // csalgo
    if (state.wntList.wntCode) {
      if ('selectaction' in data)
        state.wntList.curState = data.selectaction
      if('result' in data && data.result.toLowerCase()=='fail') {
        state.wntList.feedback = data.reason
        state.wntList.responseResult = null
      }
      else if ('result' in data && data.result.toLowerCase()=='success') {
        state.wntList.feedback = null
        state.wntList.responseResult = 'success'
        if(data.selectaction=='select')
          state.wntList.codes[data.code] = 's'
        else if (data.selectaction=='remove')
          state.wntList.codes[data.code] = 'u'
      }
      return {wntList: state.wntList}
    }
    // a1
    else if (Object.keys(state.wntList2).length) {
      if ('selectaction' in data && 'code' in data) {
        var code = parseInt(data.code)
        state.wntList2[code].curState = data.selectaction
        if('result' in data && data.result.toLowerCase()=='fail') {
          state.wntList2[code].feedback = 'fail'
          state.wntList2[code].responseResult = null
        }
        else if ('result' in data && data.result.toLowerCase()=='success') {
          state.wntList2[code].feedback = null
          state.wntList2[code].responseResult = 'success'
        }
        else {
          state.wntList2[code].feedback = null
          state.wntList2[code].responseResult = null
        }
      }
      return {wntList2: state.wntList2}
    }
  }
  
  sendUWarrantlist(data) {
    var userId = parseInt(this.state.userId)
    var algoName = (this.state.modules.call) ? this.state.modules.call : this.state.modules.put
    var issuer = this.state.issuer.curIssuer
    var ucode = parseInt(this.state.underlying.curUnderlying)
    
    if ('ucode' in data) ucode = parseInt(data.ucode)
    else if ('issuer' in data) issuer = data.issuer
    
    if (issuer && ucode) {
      var command = {cmd: 'uwarrantlist', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), issuer: issuer, ucode: ucode}
      sendWebsocket(JSON.stringify(command))
    }
  }
  
  // 测试集
  testData(render) {
    function test(data) {
      if (false) render(JSON.stringify(data))
    }
    // portfolio
    test({"action":"portfolio","algo_name":"kenny_s1algo","buy_price":0.125*100000000,"buytime":"20200806130826223","id":2,"quantity":40000*100000000,"recovery":true,"ref":"99999","sell_price":0.165*100000000,"sellime":"20200806131116300","ucode":0,"warrant_code":99999,"tm":1596690676300})
    test({"action":"portfolio","algo_name":"kenny_s1algo","buy_price":0.135*100000000,"buytime":"20200806130826223","id":2,"quantity":20000*100000000,"recovery":true,"ref":"99999","sell_price":0.155*100000000,"sellime":"20200806131116300","ucode":0,"warrant_code":99999,"tm":1596690676300})
    test({"action":"portfolio","algo_name":"kenny_s1algo","buy_price":0.092*100000000,"buytime":"20200806130826223","id":2,"quantity":10000*100000000,"recovery":true,"ref":"99999","sell_price":0.094*100000000,"sellime":"20200806131116300","ucode":0,"warrant_code":99999,"tm":1596690676300})
    // order
    test({"action":"order","algo_name":"s1algo_s1algo","filled_price":9200000,"filled_quantity":500000000000,"hkex_to_order_latency":18446744073706473000,"id":2,"order_price":9200000,"order_quantity":500000000000,"order_to_resp_latency":2160474,"orderid":172,"reason":"","recovery":true,"ref":"12946","side":"BUY","status":"filled","stoplost":5465000000,"tick_to_order_latency":144107,"transaction_time":"20200810095111000","ucode":941,"warrant_code":12946,"wbid":8900000,"tm":1597024271900})
    test({"action":"order","algo_name":"s1algo_s1algo","hkex_to_order_latency":18446744073707164000,"id":2,"order_price":0,"order_quantity":500000000000,"order_to_resp_latency":983908,"orderid":191,"reason":"2043 Invalid order price","recovery":true,"ref":"12946","side":"SELL","status":"cancel","stoplost":0,"tick_to_order_latency":59522,"transaction_time":"20200810095706000","ucode":0,"warrant_code":12946,"wbid":0,"tm":1597024626201})
    test({"action":"order","algo_name":"s1algo_s1algo","filled_price":9200000,"filled_quantity":500000000000,"hkex_to_order_latency":18446744073706473000,"id":2,"order_price":9200000,"order_quantity":500000000000,"order_to_resp_latency":2160474,"orderid":172,"reason":"","recovery":true,"ref":"12946","side":"SELL","status":"filled","stoplost":5465000000,"tick_to_order_latency":144107,"transaction_time":"20200810095111000","ucode":941,"warrant_code":12946,"wbid":8900000,"tm":1597024271900})
    // position
    test({"action":"order","algo_name":"s1algo_s1algo","filled_price":9200000,"filled_quantity":500000000000,"hkex_to_order_latency":18446744073706473000,"id":2,"order_price":9200000,"order_quantity":500000000000,"order_to_resp_latency":2160474,"orderid":172,"reason":"","recovery":true,"ref":"12948","side":"BUY","status":"filled","stoplost":5465000000,"tick_to_order_latency":144107,"transaction_time":"20200810095111000","ucode":941,"warrant_code":12948,"wbid":8900000,"tm":1597024271900})
    // signal
    test({"action":"signal","algo_name":"kenny_s1algo","code":1177,"detected":false,"detected_ask":0,"detectedlist":[{"warrantdesc":"25652|MBCKINF@EC2101A                         |17700000"},{"warrantdesc":"25088|JPCKINF@EC2011A                         |17200000"}],"id":2,"ref":"1177","tm":1597902254339})
    test({"action":"signal","algo_name":"kenny_s1algo","code":941,"detected":true,"detected_ask":5780000000,"detectedlist":[{"warrantdesc":"24031|JP-CMOB@EC2101B                         |13400000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12195|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"12345|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"11111|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"22222|JP-CMOB@EC2011A                         |21000000"},{"warrantdesc":"33333|JP-CMOB@EC2011A                         |21000000"}],"id":2,"ref":"941","tm":1597904514028})
    // stoplost
    test({"action":"stoplost","algo_name":"kenny_s1algo","code":3319,"id":2,"ref":"3319","stoplost":4175000000,"wbid":24400000,"tm":1597904884462})
    test({"action":"stoplost","algo_name":"kenny_s1algo","code":1548,"id":2,"ref":"3319","stoplost":4175000000,"wbid":24400000,"tm":1597904884462})
    // error
    test({"action":"marketstatus","algo_name":"kenny_s1algo","id":2,"new_action_status":2,"previous_action_status":2,"recovery":true,"ref":"uid_2_fail","tm":1597908039983})
    // postiton
  }
  
  setStates(states) {this.setState(states)}
  
  getStates() {return this.state}
  
  render() {
    let cells = []
    for (let no in this.state.config.value) {
      cells.push(
        <Cell
          key={"cell_"+no}
          no={no}
          type={this.state.config.value[no]}
          data={this.state.cells[no]}
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />
      )
    }
    var style = (this.state.recovery.isRecoveryEnd) ? 'is_finish' : 'is_loading'
    var curYear = new Date().getFullYear()
    
    var log = "", cssLog = {width: '100%', height: 400, fontSize: 13, lineHeight: 1, backgroundColor: '#fdfdfe'}
    var logError = "", cssLogError = {width: '100%', height: 60, fontSize: 13, lineHeight: 1, backgroundColor: '#fdfdfe'}
    for (var msg of this.msg) log += JSON.stringify(msg)+' \n\n\n'
    for (var msg of this.msg) {
      var msg = JSON.stringify(msg)
      if (msg.toLowerCase().indexOf('error') >= 0 || msg.toLowerCase().indexOf('fail') >= 0)
        logError += msg+' \n\n\n'
    }
    
    var isShowWarrantSelector = false, modules = this.state.modules
    if (('call' in modules) && modules.call && modules.call.includes('csalgo') || 
        ('put' in modules) && modules.put && modules.put.includes('csalgo')) {
      isShowWarrantSelector = true
    }
    
    return(
      <React.Fragment>
        <div className={classNames("custom-loader area", style)}>
          <span className={classNames("custom-loader spinner", style)}></span>
        </div>
        <div className={classNames('custom-loader content', style)}>
          <Status
            key="status"
            data={this.state.sizeReceiptData}
            data2={this.props.userId}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <MarketStatus
            key="marketStatus"
            data={this.state.marketStatus}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <BetSize
            key="betSize"
            lang={this.props.lang}
            data={this.state.betSize}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <IssuerSelector
            key="issuerSelector"
            data={this.state.issuer}
            data2={this.state.marketStatus}
            func={this.sendUWarrantlist}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <UnderlyingSelector
            key="underlyingSelector"
            data={this.state.underlyingList}
            data2={this.state.underlying}
            data3={this.state.underlyingDefault}
            func={this.sendUWarrantlist}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />

          {isShowWarrantSelector && 
          <WarrantSelector
            key="warrantSelector"
            data={this.state.wntList}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />}
          <textarea
            key="log"
            id="log"
            style={cssLog}
            value={log}
            readOnly
          />
          <textarea
            key="logError"
            id="logError"
            style={cssLogError}
            value={logError}
            readOnly
          />
          <SignalTable 
            key="SignalTable"
            data={this.state.signal}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <Position
            key="position"
            data={this.state.positions}
            data2={this.state.wntPrice}
            data3={this.state.codeMapping}
            data4={this.state.stockPrice}
            data5={this.state.positionActions}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <Portfolio
            key="portfolio"
            data={this.state.portfolios}
            data2={this.state.codeMapping}
            data3={this.state.wntList2}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <OrderList
            key="orderList"
            data={this.state.orders}
            data2={this.state.codeMapping}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
        </div>
        <div className="footer text-center">
          Copyright © {curYear} Fast Trader v1.0.23
        </div>
      </React.Fragment>
      /*
      <div className='row'>{cells}</div>
       <Selector
        key="selector_cbbc"
        config={this.state.config}
        setStates={this.setStates}
        getStates={this.getStates}
      />*/
    )
  }
}

ReactDOM.render(
  <FullAuto 
    config={config}
    lang={global.cookies[lang]}
    userId={userId}
    underlyingDefault={underlyingDefault}
  />, 
document.getElementById('fullAuto'))
