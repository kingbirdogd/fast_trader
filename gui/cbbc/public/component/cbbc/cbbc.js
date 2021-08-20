class Cbbc extends React.Component {
  constructor(props) {
    super(props)
    this.setStates = this.setStates.bind(this)
    this.getStates = this.getStates.bind(this)
    
    this.state = {}
    
    this.state.config = {
      defaultValue: ['bull', 'bear']
    }
    
    this.state.setting = {}
    this.state.modules = {bull: null, bear: null}
    this.state.prefix = 'u000_'
    this.state.userId = parseInt(Cookies.get("cbbc-userId"))
    
    this.state.recovery = {}
    this.state.recovery.isRecoveryEnd = false
    
    this.state.sizeReceiptData = {totalBytes: 0, noPackage: 0, lastAliveTime: null}
    
    var cells = []
    var orders = []
    var positions = []
    var portfolios = []
    
    // 生成数据集
    this.state.noCells = 100
    for(var i=0; i<this.state.noCells; i++) {
      cells[i] = {}
      
      cells[i].action = {
        code: {value: '', feedback: '', valid: 'number'},
        symbol: {value: '', feedback: '', valid: ''}, 
        issuerSize: {value: '', feedback: '', valid: '', responseResult: ''}, 
        quantity: {value: '', feedback: '', valid: '', responseResult: ''},
        spread: {value: '', feedback: '', valid: 'number'},
        delta: {value: '', feedback: '', valid: ''},
        status: {isSet: false, isStart: false, isPause: false, isStop: false, needToSetAgain: false, result: ''},
      }
      
      cells[i].priceTable = {}
      
      cells[i].wPrice = {bid: null, ask: null, diffpt: null, ipriceBid: null, ipriceAsk: null, lvlbid: null}
      
      cells[i].setting = {
        inout: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        wintick: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        inlvl: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        stoplost: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        ptrange: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        buyoffset: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        selloffset: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
        showpt: {value: '0', defaultValue: '', feedback: '', responseResult: '', valid: 'number_except_zero'},
      }
      
      cells[i].trade = {
        price: {value: '', feedback: '', valid: 'number_except_zero'},
        size: {value: '', feedback: '', valid: 'string'}
      }
      
      cells[i].info = {issuer: '', ucode: '', uname: ''}
      
      orders[i] = []
      positions[i] = []
      portfolios[i] = []
    }
    
    this.state.cells = cells
    this.state.orders = orders
    this.state.positions = positions
    this.state.portfolios = portfolios
    this.state.issuerList = getIssuer()
    this.state.cellsConfig = []
    
    this.state.visible = {recommender: false, btnRecommender: true}
  }
  
  componentDidMount() {
    const render = (res) => {
      // 記錄狀態
      this.state.sizeReceiptData.totalBytes += res.length*2
      this.state.sizeReceiptData.noPackage += 1
      this.state.sizeReceiptData.lastAliveTime = Date.now()
      
      var data = JSON.parse(res)
      if (data.action != 'connect_alive')
        console.log(data)
      var obj = $.extend(true, {}, this.state)
      if ('user_id' in data && (!obj.userId))
        obj = {userId: data.user_id}
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
      // 接口v2
      else if ('action_type' in data) {
        if (data.action_type=='start') {obj = this.setStart(obj, data)}
        else if (data.action_type=='pause') {obj = this.setPause(obj, data)}
        else if (data.action_type=='stop') {obj = this.setStop(obj, data)}
      }
      // 接口v2
      else if ('cmd' in data) {
        if (data.cmd=='get_algo_names') {obj = this.setAlgoV2(obj, data)}
        else if (data.cmd=='get_warrant_detail') {obj = this.setWarrantDetail(obj, data)}
      }
      else if ('type' in data) {
        // 接口v2
        if (data.type=='set position') {obj = this.checkPositionV2(obj, data)}
        // 公共
        else if (data.type=='recovery_end') {obj = this.setRecoveryEnd(obj, data)}
      }
      this.setState(obj)
    }
    initWebsocket(render)
    initTablePrice()
    
    async function getUserSetting(that) {
      var data = await $.ajax({url: '/api/users/'+global.cookies['cbbc-uname'], type: 'GET'})
      var obj = $.extend(true, {}, that.getStates())
      if (data.result=='success') {
        that.setState({setting: data.data})
        if ('isShowRecommend' in data.data && data.data.isShowRecommend.toLowerCase() == 'true') {
          // obj.visible.btnRecommender = true
          // that.setState({visible: obj.visible})
        }
      }
    }
    getUserSetting(this)
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
      state.cells[id].action.delta.value = pair.pair.delta.toFixed(4)
      state.cells[id].action.status.isSet = true
      state.cells[id].action.status.isPause = false
      state.cells[id].action.status.isStart = false
      state.cells[id].action.status.isStop = false
      state.cells[id].action.status.result = ''
      if (pair.pair.wtype == 1)
        state.cells[id].type = 'bull'
      else if (pair.pair.wtype == 2)
        state.cells[id].type = 'bear'
      
      state.cells[id].info.issuer = pair.pair.wname.substring(0, 2)
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
    for (var i in state.cellsConfig) {
      var code = state.cells[i].action.code.value
      // 已输入编号, 才要报价表
      if (code.length > 0) {
        var command = ['loadpricetable', code, state.prefix+i]
        var command1 = {type: "algo_command", "key": state.modules[state.cellsConfig[i].type], command: command.join('|')}
        sendWebsocket(JSON.stringify(command1))
      }
    }
    
    return {modules: state.modules}
  }
  
  // Algo v2
  setAlgoV2(state, data) {
    for (var algo in data.algos)
      if (algo.includes('bear'))
        state.modules.bull = algo, state.modules.bear = algo
    // 沒有cbbc algo
    if ( (state.modules.bull != null && state.modules.bull.includes('bear')) && (state.modules.bear != null && state.modules.bear.includes('bear')) )
      console.log({log: 'algo exist'})
    // 报价表
    async function loadPrice(state) {
      for (var i in state.cellsConfig) {
        var code = state.cells[i].action.code.value
        var wtype = state.cellsConfig[i].type
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
            
            var command2 = {
              algo_name: state.modules[wtype],
              cmd: "get_warrant_detail",
              code: parseInt(code),
              id: parseInt(state.userId),
              no: i
            }
            sendWebsocket(JSON.stringify(command2))
            
          }
          else if (!algo)
            console.log('Algo not found. Cannot load price table.')
        }
      }
    }
    loadPrice(state)
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
    var id = data.ref.replace(state.prefix, '')
    var arr = {
      ref:data.ref, mode:data.id, key: data.algo_name,
      code: data.warrant_code, quantity: formatLong(data.quantity),
      buyPrice: formatLong(data.buy_price), buyTime: formatDate(data.buytime),
      sellPrice: formatLong(data.sell_price), soldTime: formatDate(data.sellime),
      profitLoss: (formatLong(data.sell_price)-formatLong(data.buy_price))*formatLong(data.quantity),
      issuer: data.issuer, wtype: data.wtype
    }
    if(!(id in state.portfolios))
      state.portfolios[id] = []
    state.portfolios[id].push(arr)
    return {portfolios: state.portfolios}
  }
  
  // 買賣
  setOnOrder(state, data) {
    var id = data.ref.replace(state.prefix, '')
    var arr = {
      code: data.warrant_code, 
      // side: data.action.toLowerCase(),
      side: data.side.toLowerCase(),
      status: data.status,
      transactionTm: ((data.transaction_time) ? formatDate(data.transaction_time) : getCurDateTime()),
      matchPrice: formatLong(data.filled_price), 
      matchQuantity: formatLong(data.filled_quantity),
      totalPrice: formatLong(data.filled_price)*formatLong(data.filled_quantity),
      futurePrice: ('sellout' in data) ? formatPrice(data.sellout) : ('buyin' in data) ? formatPrice(data.buyin) : '',
      reason: ('reason' in data) ? data.reason: '',
      wtype: data.wtype,
      issuer: data.issuer
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
    return state
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
    
    // 影射
    var last = 0
    for (var i in state.cells)
      if (state.cells[i].action.code.value)
        last = parseInt(i)+1
    
    for(var i=0; i<last; i++) {
      var v = state.cells[i]
      if (v.action.code.value)
        state.cellsConfig.push({code: v.action.code.value, type: v.type, isVisable: false})
      else
        state.cellsConfig.push({code: '', type: 'undefined', isVisable: false})
    }
    return state
  }
  
  // 標的掛牌價
  setWprice(state, data) {
    var id = data.ref.replace(state.prefix, ''), side = data.side.toLowerCase(), wprice = formatLong(data.wprice)
    state.cells[id].wPrice[side] = wprice
    
    state.cells[id].wPrice.buyin = formatPrice(data.buyin)
    state.cells[id].wPrice.sellout = formatPrice(data.sellout)
    state.cells[id].wPrice.diffask = formatPrice(data.diffask)
    state.cells[id].wPrice.diffbid = formatPrice(data.diffbid)
    
    if ('diffpt' in data && data.diffpt < 99999999)
      state.cells[id].wPrice.diffpt = formatPrice(data.diffpt)
    if ('lvlbid' in data && data.lvlbid != 99999999)
      state.cells[id].wPrice.lvlbid = formatPrice(data.lvlbid)
    
    if (data.side.toLowerCase() == 'ask')
      state.cells[id].wPrice.ipriceAsk = formatLong(data.iprice)
    else if (data.side.toLowerCase() == 'bid')
      state.cells[id].wPrice.ipriceBid = formatLong(data.iprice)
    
    return {cells: state.cells}
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
  
  // 
  setWarrantDetail(state, data) {
    if ('code' in data && 'CallPut' in data && 'underlying' in data && 'underlying_price' in data && 'warrant_price' in data) {
      if ('symbols' in data && data.symbols.length > 0)
        state.cells[data.no].action.symbol.value = data.symbols[0]
      state.cells[data.no].info.ucode = data.underlying
      
      if (getUnderlyingName2(data.underlying) != '')
        state.cells[data.no].info.uname = getUnderlyingName2(data.underlying)
      else
        state.cells[data.no].info.uname = data.underlying
      
      state.cells[data.no].wPrice.ask = formatPrice2(data.warrant_price.m_Ask.m_iPrice)
      state.cells[data.no].wPrice.bid = formatPrice2(data.warrant_price.m_Bid.m_iPrice)
    }
    return {cells: state.cells}
  }
  
  // 強制登出
  connectReject(data) {
    alert(data.error);
    global.func.logout();
  }
  
  setStates(states) {this.setState(states)}
  
  getStates() {return this.state}
  
  render() {
    let cells = [], wprices = []
    for (let no in this.state.cellsConfig) {
      if (this.state.cellsConfig[no].isVisable == true) {
        cells.push(
          <Cell
            key={"cell_"+no}
            no={no}
            type={this.state.cellsConfig[no].type}
            data={this.state.cells[no]}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
        )
      }
      wprices.push(this.state.cells[no].wPrice)
    }
    var style = (this.state.recovery.isRecoveryEnd) ? 'is_finish' : 'is_loading'
    var curYear = new Date().getFullYear()
    return(
      <React.Fragment>
        <div className={classNames("custom-loader area", style)}>
          <span className={classNames("custom-loader spinner", style)}></span>
        </div>
        <div className={classNames('custom-loader content', style)}>
          <Status
            key="status"
            data={this.state.sizeReceiptData}
            data2={this.state.visible}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          {this.state.visible.recommender &&
          <Recommender
            key="recommender"
            data={this.state.issuerList}
            data2={this.state.cellsConfig}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />}
          <TradeTable
            key="tradeTable"
            data={this.state.cells}
            data2={this.state.cellsConfig}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <div className='row'>{cells}</div>
          <Position
            key="position"
            data={this.state.positions}
            data2={wprices}
            data3={this.state.cellsConfig}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <Portfolio
            key="portfolio"
            data={this.state.portfolios}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
          <OrderList
            key="orderList"
            data={this.state.orders}
            lang={this.props.lang}
            setStates={this.setStates}
            getStates={this.getStates}
          />
        </div>
        <div className="footer text-center">
          Copyright © {curYear} Fast Trader v1.0.23
        </div>
      </React.Fragment>
      /*<Selector
        key="selector_cbbc"
        config={this.state.config}
        setStates={this.setStates}
        getStates={this.getStates}
      />
      <Command
        key="command"
        lang={this.props.lang}
        setStates={this.setStates}
        getStates={this.getStates}
      />*/
    )
  }
}

var lang = global.cookies['cbbc-lang']
ReactDOM.render(<Cbbc lang={lang} />, document.getElementById('cbbc'))
