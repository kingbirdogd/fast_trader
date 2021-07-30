class Ticket extends React.Component {
  constructor(props) {
    super(props)
    this.setStates = this.setStates.bind(this)
    this.getStates = this.getStates.bind(this)
    
    this.state = {}
    this.state.visible = {position: false, order: false, command: false, recommender: false}
    this.state.modules = {bear: undefined, bull: undefined, call: undefined, put: undefined}
    this.state.noCell = {cur: 8, max: 12}
    this.state.sizeReceiptData = {totalBytes: 0, noPackage: 0, lastAliveTime: undefined}
    this.state.portfolio = []
    this.state.orders = []
    this.state.positions = {}
    this.state.setting = {}
    this.state.issuerList = getIssuer()
    this.state.render = undefined
    this.state.instance = () => {
      return {
        wnt: {
          code: {code: undefined, wtype: undefined, len: 5, status: undefined, status1: undefined}, 
          position: undefined,
          priceTable: {
            bid: {price: undefined, qty: undefined, noOrder: undefined, ratio: undefined},
            ask: {price: undefined, qty: undefined, noOrder: undefined, ratio: undefined},
          },
          buy: {price: undefined, qty: undefined, status: undefined},
          sell: {price: undefined, qty: undefined, status: undefined, max: undefined, min: undefined},
          stopLoss: {price: undefined, status: undefined},
          msg: [],
        },
        stock: {
          code: {code: undefined, len: 4, status: undefined},
          priceTable: {
            bid: {price: undefined, qty: undefined, noOrder: undefined, ratio: undefined},
            ask: {price: undefined, qty: undefined, noOrder: undefined, ratio: undefined},
          },
          buy: {price: undefined, qty: undefined, ratio: undefined},
          sell: {price: undefined, qty: undefined, ratio: undefined, max: undefined, min: undefined},
          status1: undefined,
          status2: undefined,
          status3: undefined,
          status4: undefined,
          action1: undefined,
          action2: undefined,
          action3: undefined,
          action4: undefined,
          nTrack:  undefined,
          aTrack: undefined,
          xTrack: undefined,
          pTrack: undefined,
          tick: undefined,
        },
        config: {
          layout: 'normal',
        },
        updatePrice: {isUpdate1: false, isUpdate2: false},
      }
    }
    
    this.state.cells = []
    for (var i=0; i<this.state.noCell.max; i++)
      this.state.cells.push(this.state.instance())
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
      if ('user_id' in data && (!obj.userId))
        obj = {userId: data.user_id}
      
      else if ('admin_message' in data)
        alert(data.admin_message)
      
      else if ('cmd' in data) {
        if (data.cmd == 'get_algo_names') {obj = this.setAlgo(obj, data)}
        else if (data.cmd == 'get_warrant_detail') {obj = this.setWarrantDetail(obj, data)}
      }
      
      else if ('action' in data) {
        if (data.action=='connect_reject') {this.connectReject(data)}
        else if (data.action == 'exception' && data.msg_type == 'semipro_algo_err_msg') {obj = this.setError(obj, data)}
        else if (data.action == 'stoplost') {obj = this.setStoploss(obj, data)}
      }
      
      else if ('msg_type' in data) {
        if (data.msg_type == 'semipro_algo_get') {obj = this.setWarrantDetail2(obj, data)}
        else if (data.msg_type == 'semipro_algo_set') {obj = this.setAlgoSet(obj, data)}
        else if (data.msg_type == 'semipro_algo_force_buy') {obj = this.setForceBuy(obj, data)}
        else if (data.msg_type == 'semipro_algo_force_sell') {obj = this.setForceSell(obj, data)}
        else if (data.msg_type == 'semipro_algo_odr_msg') {obj = this.setOdr(obj, data)}
        else if (data.msg_type == 'semipro_algo_odr_position') {obj = this.setPosition(obj, data)}
        else if (data.msg_type == 'algo_getprofit_msg') {obj = this.setProfit(obj, data)}
        else if (data.msg_type == 'semi_algo_cancel') {obj = this.setCancel(obj, data)}
        else if (data.msg_type == 'semi_algo_limit_sell') {obj = this.setLimitSet(obj, data)}
        else if (data.msg_type == 'semi_algo_limit_modify') {obj = this.setLimitModify(obj, data)}
      }
      
      else if ('ref' in data) {
        if (data.ref = 'orders') {obj = this.setOrder(obj, data)}
      }
      
      else if ('type' in data) {
        if (data.type=='recovery_end') {obj = this.setRecoveryEnd(obj, data)}
      }
      this.state.render = render
      this.setState(obj)
    }
    initWebsocket(render)
    
    async function getUserSetting(that) {
      var data = await $.ajax({url: '/api/users/'+global.cookies['semipro-uname'], type: 'GET'})
      if (data.result=='success') {
        that.setState({setting: data.data})
        // 票式數量
        if ('windowMax' in data.data && 'windowShow' in data.data)
          that.setState({noCell: {cur: data.data.windowShow, max: data.data.windowMax}})
      }
    }
    getUserSetting(this)
  }
  
  setAlgo(obj, data) {
    for (var algo in data.algos)
      if (algo.includes('semipro'))
        obj.modules.call = algo, obj.modules.put = algo
    // 沒有cbbc algo
    if ( (obj.modules.call != null && obj.modules.call.includes('semipro')) && (obj.modules.put != null && obj.modules.put.includes('semipro')) )
      console.log({log: 'algo exist'})
    
    // 初始化
    var userId = parseInt(obj.userId)
    var algoName = obj.modules.call
    
    for(var i=0; i<obj.noCell.max; i++) {
      var command = {cmd: 'get', id: userId, algo_name: algoName, ref: setNo(i)}
      sendWebsocket(JSON.stringify(command))
    }
    var command1 = {cmd: "order_list", id: userId, algo_name: algoName, max_display: 9999, ref: 'orders'}
    sendWebsocket(JSON.stringify(command1))
    var command2 = {cmd: 'getprofit', id: userId, algo_name: algoName, ref: 'getprofit'}
    sendWebsocket(JSON.stringify(command2))
    var command3 = {cmd: 'position', id: userId, algo_name: algoName, ref: 'position'}
    sendWebsocket(JSON.stringify(command3))
    
    return obj
  }
  
  setWarrantDetail(obj, data) {
    if ('error' in data && 'code' in data && 'no' in data) {
      obj.cells[data.no].wnt.code.status = 'error'
      var time = moment(data.tm).format("hh:mm:ss")
      obj.cells[data.no].wnt.msg[0] = time+' '+data.error
      return obj
    }
    else if ('code' in data && 'CallPut' in data && 'underlying' in data && 'underlying_price' in data && 'warrant_price' in data) {
      var status1 = obj.cells[data.no].wnt.code.status1
      
      // 指數
      if (isNaN(parseInt(data.underlying))) {
        data.underlying_price.m_Ask.m_iPrice *= 1000
        data.underlying_price.m_Bid.m_iPrice *= 1000
        data.pricemark.buyin *= 1000
        data.pricemark.sellout *= 1000
      }
      
      // 正股
      if (typeof status1=='undefined' || status1=='recover' || status1=='xTrack' || status1=='aTrack') {
        var ratio1 = data.underlying_price.m_Ask.m_uQuantity+data.underlying_price.m_Bid.m_uQuantity
        obj.cells[data.no].stock.code.code = data.underlying
        obj.cells[data.no].stock.priceTable = {
          bid: {
            price: formatPrice2(data.underlying_price.m_Ask.m_iPrice),
            qty: data.underlying_price.m_Ask.m_uQuantity,
            noOrder: data.underlying_price.m_Ask.m_uNumberOfOrder,
            ratio: (data.underlying_price.m_Ask.m_uQuantity/ratio1)*100},
          ask: {
            price: formatPrice2(data.underlying_price.m_Bid.m_iPrice),
            qty: data.underlying_price.m_Bid.m_uQuantity,
            noOrder: data.underlying_price.m_Bid.m_uNumberOfOrder,
            ratio: (data.underlying_price.m_Bid.m_uQuantity/ratio1)*100},
        }
      }
      
      // 正股 觸買
      if (typeof status1 == 'undefined' || status1=='recover') {
        obj.cells[data.no].stock.status1 = 'close'
        obj.cells[data.no].stock.status2 = 'open'
        obj.cells[data.no].stock.status3 = 'close'
        obj.cells[data.no].stock.status4 = 'close'
      }
      
      // 輪
      if (typeof status1 == 'undefined' || status1=='recover' || status1=='nTrack' || status1=='aTrack') {
        obj.cells[data.no].wnt.code.status = 'success'
        var ratio2 = data.warrant_price.m_Ask.m_uQuantity+data.warrant_price.m_Bid.m_uQuantity
        obj.cells[data.no].wnt.code.wtype = data.CallPut.toLowerCase()
        obj.cells[data.no].wnt.priceTable = {
          bid: {
            price: formatPrice2(data.warrant_price.m_Ask.m_iPrice),
            qty: data.warrant_price.m_Ask.m_uQuantity,
            noOrder: data.warrant_price.m_Ask.m_uNumberOfOrder,
            ratio: (data.warrant_price.m_Ask.m_uQuantity/ratio2)*100},
          ask: {
            price: formatPrice2(data.warrant_price.m_Bid.m_iPrice),
            qty: data.warrant_price.m_Bid.m_uQuantity,
            noOrder: data.warrant_price.m_Bid.m_uNumberOfOrder,
            ratio: (data.warrant_price.m_Bid.m_uQuantity/ratio2)*100},
        }
      }
      
      // 持貨量
      var position = 0
      if (data.code in obj.positions)
        position = formatInputUnit(obj.positions[data.code], true)

      // 輪 輸入框
      var isUpdate1 = obj.cells[data.no].updatePrice.isUpdate1
      var isUpdate2 = obj.cells[data.no].updatePrice.isUpdate2
      
      if ((typeof status1 == 'undefined' || status1 == 'nTrack' || status1 == 'aTrack') && (isUpdate1 == false && isUpdate2 == false)) {
        obj.cells[data.no].wnt.buy.price = formatPrice2(data.warrant_price.m_Ask.m_iPrice)
        if (position<=0) {}
        obj.cells[data.no].wnt.sell.price = formatPrice2(data.warrant_price.m_Bid.m_iPrice)
        if (obj.cells[data.no].wnt.stopLoss.status != 'start') {
          obj.cells[data.no].wnt.stopLoss.price = formatPrice2(data.warrant_price.m_Bid.m_iPrice)
        }
      }
      if (typeof status1 == 'undefined') {
        var qty = 10000
        if (position<=0) {
          obj.cells[data.no].wnt.buy.qty = formatInputUnit(qty, false)
        }
        obj.cells[data.no].wnt.sell.qty = formatInputUnit(qty, false)
      }
      // pTrack - 輪&正股 輸入框
      if (status1 == 'pTrack' && (isUpdate1 == false && isUpdate2 == false)) {
        // 正股 - 有價
        if ('pricemark' in data && data.pricemark.buyin > 0 && data.pricemark.sellout < 99999999999) {
          if (data.CallPut.toLowerCase() == 'p') {
            obj.cells[data.no].stock.sell.price = formatLong(data.pricemark.buyin)
            obj.cells[data.no].stock.buy.price = formatLong(data.pricemark.sellout)
          }
          if (data.CallPut.toLowerCase() == 'c') {
            obj.cells[data.no].stock.buy.price = formatLong(data.pricemark.buyin)
            obj.cells[data.no].stock.sell.price = formatLong(data.pricemark.sellout)
          }
        }
        // 正股 - 沒價
        else {
          obj.cells[data.no].wnt.msg[0] = getTime()+' Pricemark not found.'
        }
        // 輪
        if ('pricemark' in data) {
          obj.cells[data.no].wnt.buy.price = formatLong(data.pricemark.wask)
          obj.cells[data.no].wnt.sell.price = formatLong(data.pricemark.wbid)
        }
      }
      // 正股 輸入框
      if ((typeof status1 == 'undefined' || status1 == 'xTrack' || status1 == 'aTrack') && (isUpdate1 == false && isUpdate2 == false)) {
        if (data.CallPut.toLowerCase() == 'p') {
          obj.cells[data.no].stock.buy.price = formatPrice2(data.underlying_price.m_Bid.m_iPrice)
          if (position<=0 || obj.cells[data.no].stock.status4 == 'open') {}
          obj.cells[data.no].stock.sell.price = formatPrice2(data.underlying_price.m_Ask.m_iPrice)
        }
        else if (data.CallPut.toLowerCase() == 'c') {
          obj.cells[data.no].stock.sell.price = formatPrice2(data.underlying_price.m_Bid.m_iPrice)
          if (position<=0 || obj.cells[data.no].stock.status4 == 'open') {}
          obj.cells[data.no].stock.buy.price = formatPrice2(data.underlying_price.m_Ask.m_iPrice)
        }
      }
      if (typeof status1 == 'undefined') {
        var qty = 0
        obj.cells[data.no].stock.buy.qty = formatInputUnit(qty, false)
        obj.cells[data.no].stock.sell.qty = formatInputUnit(qty, false)
        var ratio = 0
        obj.cells[data.no].stock.buy.ratio = ratio
        obj.cells[data.no].stock.sell.ratio = ratio
      }
      
      // Track
      if (obj.cells[data.no].stock.nTrack == 'start')
        obj.cells[data.no].stock.nTrack = undefined
      if (obj.cells[data.no].stock.aTrack == 'start')
        obj.cells[data.no].stock.aTrack = undefined
      if (obj.cells[data.no].stock.xTrack == 'start')
        obj.cells[data.no].stock.xTrack = undefined
      if (obj.cells[data.no].stock.pTrack == 'start')
        obj.cells[data.no].stock.pTrack = undefined
      if (obj.cells[data.no].updatePrice.isUpdate1 == true)
        obj.cells[data.no].updatePrice.isUpdate1 = false
        
      
      obj.cells[data.no].wnt.code.status1 = 'complete'
      return obj
    }
    else
      return obj
  }
  
  setWarrantDetail2(obj, data) {
    if ('result' in data && data.result.toLowerCase() && 'ref' in data && data.ref && 'pair' in data && data.pair) {
      var no = getNo(data.ref)
      // 正股
      obj.cells[no].stock.code.code = parseInt(data.pair.underlying_code)
      obj.cells[no].stock.buy.price = formatLong(data.pair.buy_trriger)
      obj.cells[no].stock.sell.price = formatLong(data.pair.sell_trriger)
      
      if (data.pair.max_sell_trriger > 0 && data.pair.min_sell_trriger > 0) {
        obj.cells[no].stock.sell.price = formatLong(data.pair.max_sell_trriger)
        obj.cells[no].stock.sell.max = formatLong(data.pair.max_sell_trriger)
        obj.cells[no].stock.sell.min = formatLong(data.pair.min_sell_trriger)
      }

      obj.cells[no].stock.buy.qty = formatInputUnit(formatLong(data.pair.early_buy_qty), false)
      obj.cells[no].stock.sell.qty = formatInputUnit(formatLong(data.pair.early_sell_qty), false)

      obj.cells[no].stock.buy.ratio = formatLong(data.pair.ratio_buy)
      obj.cells[no].stock.sell.ratio = formatLong(data.pair.ratio_sell)
      // 輪
      obj.cells[no].wnt.code.status1 = 'recover'
      var wntCode = parseInt(data.pair.warrant_code)
      obj.cells[no].wnt.code.code = wntCode
      // 輪 輸入框
      obj.cells[no].wnt.buy.price = formatLong(data.pair.buy_price)
      obj.cells[no].wnt.sell.price = formatLong(data.pair.sell_price)
      
      if (data.pair.max_sell_price > 0 && data.pair.min_sell_price > 0) {
        obj.cells[no].wnt.sell.price = formatLong(data.pair.max_sell_price)
        obj.cells[no].wnt.sell.max = formatLong(data.pair.max_sell_price)
        obj.cells[no].wnt.sell.min = formatLong(data.pair.min_sell_price)
      }
      
      if (formatLong(data.pair.bottom_price)<=0.001 || formatLong(data.pair.position)==0)
        obj.cells[no].wnt.stopLoss.price = formatLong(data.pair.buy_price)
      else {
        obj.cells[no].wnt.stopLoss.price = formatLong(data.pair.bottom_price)
        obj.cells[no].wnt.stopLoss.status = 'start'
      }
      
      obj.cells[no].wnt.buy.qty = formatInputUnit(formatLong(data.pair.auto_buy_quantity), false)
      obj.cells[no].wnt.sell.qty = formatInputUnit(formatLong(data.pair.auto_buy_quantity), false)
      obj.cells[no].wnt.position = formatInputUnit(formatLong(data.pair.position), false)
      // 操作
      if (data.no_change == true)
        var a=1
      else if (data.pair.auto_buy == true || data.pair.auto_sell == true) {
        obj.cells[no].stock.action1  = 'start'
        if (data.pair.max_sell_price > 0 && data.pair.min_sell_price > 0)
          obj.cells[no].stock.action3  = 'start'
      }
      else
        obj.cells[no].stock.action1  = 'stop'
      
      // 買賣
      if (data.pair.auto_buy == true) obj.cells[no].wnt.buy.status = 'open'
      if (data.pair.auto_sell == true) obj.cells[no].wnt.sell.status = 'open'
      
      // 報價
      var command = {cmd: 'get_warrant_detail', code: wntCode, algo_name: obj.modules.call, id: obj.userId, no: no}
      sendWebsocket(JSON.stringify(command))
      
      // 顯示格數
      if (no+1 > obj.noCell.cur) obj.noCell.cur = no+1
    }
    return obj
  }
  
  setAlgoSet(obj, data) {
    var no = getNo(data.ref)
    if ('result' in data && 'pair' in data && data.result.toLowerCase()=='success') {
      // 操作
      if (data.no_change == true)
        var a=1
      else if (data.pair.auto_buy == true || data.pair.auto_sell == true) {
        obj.cells[no].stock.action1  = 'start'
        
        if (data.pair.auto_buy == true && data.pair.auto_sell == true)
          obj.cells[no].wnt.msg[0] = getTime()+' Auto start'
        else if (data.pair.auto_buy == true)
          obj.cells[no].wnt.msg[0] = getTime()+' Buy start'
        else if (data.pair.auto_sell == true)
          obj.cells[no].wnt.msg[0] = getTime()+' Sell start'
      }
      else
        obj.cells[no].stock.action1  = 'stop', obj.cells[no].wnt.msg[0] = getTime()+' Stop'
      
      // 買賣
      if (data.pair.auto_buy == true) obj.cells[no].wnt.buy.status = 'open'
      if (data.pair.auto_sell == true) obj.cells[no].wnt.sell.status = 'open'
    }
    else if (data.result.toLowerCase() == "can't change action while buying / selling.") {
      obj.cells[no].stock.action1  = 'fail'
      obj.cells[no].wnt.msg[0] = getTime()+' '+data.result.toLowerCase()
    }
    else {
      obj.cells[no].stock.action1  = 'fail'
      obj.cells[no].wnt.msg[0] = getTime()+' Fail to start algo.'
    }
    return obj
  }
  
  setOdr(obj, data) {
    var no = getNo(data.ref)
    if ('auto_buy' in data && 'auto_sell' in data && 'status' in data && 'position' in data && 'side' in data) {
      if (data.auto_buy == false && data.auto_sell == false && data.status.toLowerCase() == 'filled') {
        obj.cells[no].stock.action1 = 'stop'
        
        // 检查stoploss
        if (parseFloat(data.position) == 0) {
          obj.cells[no].wnt.sell.status = 'close'
          
          obj.cells[no].wnt.stopLoss.status = 'stop'
          // obj.cells[no].wnt.stopLoss.price = obj.cells[no].wnt.priceTable.bid.price
          
          obj.cells[no].stock.action2 = undefined
          obj.cells[no].stock.action3 = undefined
        }
        // 買盤
        else if (parseFloat(data.position) > 0) {
          obj.cells[no].wnt.buy.status = 'close'
        }
      }
      
      if (data.status.toLowerCase() == 'filled')
        obj.cells[no].wnt.position = formatInputUnit(formatLong(data.position), false)

      if (data.status.toLowerCase() == 'filled' && data.side.toLowerCase() == 'buy')
        obj.cells[no].wnt.msg[0] = getTime()+' Order Bought', obj.cells[no].wnt.buy.status = 'close'
      else if (data.status.toLowerCase() == 'filled' && data.side.toLowerCase() == 'sell')
        obj.cells[no].wnt.msg[0] = getTime()+' Order Sold', obj.cells[no].wnt.sell.status = 'close'
    }
    var userId = obj.userId
    var algoName = obj.modules.call
    // 倉位
    var command1 = {cmd: 'position', id: userId, algo_name: algoName, ref: 'position'}
    sendWebsocket(JSON.stringify(command1))
    // 單
    var command2 = {cmd: "order_list", id: userId, algo_name: algoName, max_display: 9999, ref: 'orders'}
    sendWebsocket(JSON.stringify(command2))
    // 利潤
    var command3 = {cmd: 'getprofit', id: userId, algo_name: algoName, ref: 'getprofit'}
    sendWebsocket(JSON.stringify(command3))
    
    return obj
  }
  
  setPosition(obj, data) {
    if ('position' in data && data.position) {
      for (const [k, v] of Object.entries(data.position)) {
        //
        for(var k1 in obj.cells) {
          if(obj.cells[k1].wnt.code.code == k){}
            //obj.cells[k1].wnt.position = formatInputUnit(formatLong(v), false)
        }
        // 
        obj.positions[k] = formatInputUnit(formatLong(v), false)
      }
    }
    return obj
  }
  
  setProfit(obj, data) {
    if ('portfolio' in data) {
      obj.portfolio = []
      for (var p of data.portfolio) {
        obj.portfolio.push({
          averagebuy: formatPrice(p.averagebuy),
          averagesell: formatPrice(p.averagesell),
          buyturnover: parseFloat(p.buyturnover)/1000000,
          buyvolume: formatPrice(p.buyvolume),
          code: p.code,
          profit: parseFloat(p.profit)/1000000,
          sellturnover: parseFloat(p.sellturnover)/1000000,
          sellvolume:  formatPrice(p.sellvolume)
        })
      }
    }
    return obj
  }
  
  setError(obj, data) {
    var no = getNo(data.ref)
    if ('error' in data && data.error) {
      obj.cells[no].stock.action1 = 'fail'
      obj.cells[no].wnt.msg[0] = getTime()+' '+data.error
    }
    return obj
  }
  
  setOrder(obj, data) {
    if ('orders' in data) {
      obj.orders = []
      for (var order of data.orders) {
        obj.orders.push({
          time: formatDate(order.transaction_tm),
          status: order.status.replace('order_status::', ''),
          orderId: order.header.order_id,
          code: order.code,
          side: order.side.replace('order_side::', ''),
          quantity: formatLong(order.quantity),
          price: formatLong(order.price),
          matchQuantity: formatLong(order.match_quantity),
          matchPrice: formatLong(order.match_price),
          brokerId: parseFloat(order.broker_id),
          orderRef: parseFloat(order.order_ref),
          reject: (order.reject_reason == '') ? '-' : order.reject_reason
        })
        
        //
        if (!order.reject_reason == '') {
          for (var i in obj.cells) {
            if (obj.cells[i].wnt.code.code == order.code) {
              obj.cells[i].wnt.msg[0] = getTime()+' '+order.reject_reason
              // obj.cells[i].stock.action1 = 'fail'
            }
          }
        }
      }
      
      //
      for (var order1 of obj.orders) {
        if (order1.status == 'queued') {
          for (var i in obj.cells) {
            if (order1.code == obj.cells[i].wnt.code.code) {
              obj.cells[i].wnt.stopLoss.price = order1.price
              obj.cells[i].wnt.stopLoss.status = 'start'
              obj.cells[i].stock.action1 = 'start'
            }
          }
        }
      }
    }
    return obj
  }
  
  setForceBuy(obj, data) {
    var no = getNo(data.ref)
    if ('result' in data && data.result.toLowerCase().includes('fail')) {
      obj.cells[no].wnt.msg[0] = getTime()+' Fail to force buy.'
    }
    if ('result' in data && data.result.toLowerCase() == 'success') {
      obj.cells[no].stock.action4 = undefined
      obj.cells[no].wnt.msg[0] = getTime()+' Force buy'
    }
    return obj
  }
  
  setForceSell(obj, data) {
    var no = getNo(data.ref)
    if ('result' in data && data.result.toLowerCase().includes('fail')) {
      obj.cells[no].wnt.msg[0] = getTime()+' Fail to force sell.'
    }
    else if ('result' in data && data.result.toLowerCase() == 'success') {
      obj.cells[no].stock.action4 = undefined
      obj.cells[no].wnt.msg[0] = getTime()+' Force sell'
    }
    return obj
  }
  
  setLimitSet(obj, data) {
    var no = getNo(data.ref)
    if ('result' in data && data.result.toLowerCase().includes('fail')) {
      obj.cells[no].stock.action1  = 'fail'
      obj.cells[no].wnt.stopLoss.status  = 'stop'
      obj.cells[no].wnt.msg[0] = getTime()+' Fail to place a Limited Sell Order.'
    }
    else if ('result' in data && data.result.toLowerCase().includes('success')) {
      obj.cells[no].stock.action1  = 'start'
      obj.cells[no].wnt.stopLoss.status  = 'start'
    }
    return obj
  }
  
  setLimitModify(obj, data) {
    var no = getNo(data.ref)
    if ('result' in data && data.result.toLowerCase().includes('fail')) {
      obj.cells[no].stock.action1  = 'fail'
      obj.cells[no].wnt.stopLoss.status  = 'stop'
      obj.cells[no].wnt.msg[0] = getTime()+' Fail to modify a Limited Sell Order.'
    }
    else if ('result' in data && data.result.toLowerCase().includes('success')) {
      obj.cells[no].stock.action1  = 'start'
      obj.cells[no].wnt.stopLoss.status  = 'start'
    }
    return obj
  }
  
  setCancel(obj, data) {
    var no = getNo(data.ref)
    if ('result' in data && data.result.toLowerCase() == 'fail') {
      obj.cells[no].stock.action1  = 'fail'
      obj.cells[no].wnt.stopLoss.status  = 'stop'
      obj.cells[no].wnt.msg[0] = getTime()+' Fail to cancel a Limited Sell Order.'
    }
    else if ('result' in data && data.result.toLowerCase() == 'success') {
      obj.cells[no].stock.action1  = 'stop'
      obj.cells[no].wnt.stopLoss.status  = 'stop'
    }
    return obj
  }
  
  setStoploss(obj, data) {
    var no = getNo(data.ref)
    if ('stoplost' in data && 'wbid' in data) {
      obj.cells[no].stock.sell.max = formatLong(data.stoplost)
      obj.cells[no].stock.sell.price = formatLong(data.stoplost)
      obj.cells[no].wnt.sell.max = formatLong(data.wbid)
      obj.cells[no].wnt.sell.price = formatLong(data.wbid)
      obj.cells[no].wnt.msg[0] = getTime()+' Raise StopLost to'+formatLong(data.stoplost)
    }
    return obj
  }
  
  connectReject(data) {
    alert(data.error)
    global.func.logout()
  }
  
  setRecoveryEnd(obj, data) {
    return obj
  }
  
  setStates(states) {this.setState(states)}
  
  getStates() {return this.state}
  
  render() {
    
    var cells = []
    for (var no=0; no<this.state.noCell.cur; no++) {
      cells.push(
        <Cell
          key={"cell_"+no}
          no={no}
          lang={this.props.lang}
          getInstance={this.state.instance}
          data={this.state.cells[no]}
          setStates={this.setStates}
          getStates={this.getStates}
        />
      )
    }
    
    return(
      <React.Fragment>
      
      <div className="row mb-2 mb-sm-2 control-container">
      <Control
        key={"control"}
        data1={this.state.noCell}
        data2={this.state.visible}
        lang={this.props.lang}
        setStates={this.setStates}
        getStates={this.getStates}
      />
      
      <Status
        key="status"
        data={this.state.sizeReceiptData}
        data2={this.props.userId}
        lang={this.props.lang}
        setStates={this.setStates}
        getStates={this.getStates}
      />
      </div>
      
      
      {this.state.visible.recommender &&
      <Recommender
        key="recommender"
        data={this.state.issuerList}
        lang={this.props.lang}
        setStates={this.setStates}
        getStates={this.getStates}
      />}
      
      {this.state.visible.position &&
      <Position
        key="position"
        lang={this.props.lang}
        data={this.state.positions}
        setStates={this.setStates}
        getStates={this.getStates}
      />}
      
      {this.state.visible.order &&
      <Order
        key="order"
        lang={this.props.lang}
        data={this.state.orders}
        data2={this.state.portfolio}
        setStates={this.setStates}
        getStates={this.getStates}
      />}
      
      {this.state.visible.command &&
      <Command
        key="command"
        lang={this.props.lang}
        setStates={this.setStates}
        getStates={this.getStates}
      />}
      
      <div className="row">
      {cells}
      </div>
      
      </React.Fragment>
    )
  }
}

ReactDOM.render(
  <Ticket 
    lang={global.cookies[lang]}
    userId={userId}
  />, 
document.getElementById('ticket'))