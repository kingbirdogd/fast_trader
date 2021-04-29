class InputGroupTicket extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
    this.handleChange = this.handleChange.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.initData = this.initData.bind(this)
    this.setter = this.setter.bind(this)
    this.handleSend = this.handleSend.bind(this)
  }
  
  componentDidMount() {

  }
  
  handleSend(that, obj) {
    var {name, no, key, states, userId, algoName} = that.initData(that, event)
    
    var command1 = that.props.initInputData(obj, no, userId, algoName, 'set', obj.wnt.stopLoss.status).command
    if (key == 'wnt.stopLoss.price' && (obj.stock.action2 == 'start' || obj.stock.action3 == 'start')) {
      command1.action = 'NOCHANGE'
      sendWebsocket(JSON.stringify(command1))
    }
    else if (obj.stock.action1 == 'start') {
      command1.action = 'STOP'
      sendWebsocket(JSON.stringify(command1))
    }
    else if (obj.stock.action1 == 'stop') {
      command1.action = 'STOP'
      sendWebsocket(JSON.stringify(command1))
    }
  }
  
  handleChange() {
    var {name, val, no, key, states, obj} = this.initData(this, event)
    
    // 1.0 檢查
    if (key.includes('price') || key.includes('ratio'))
      val = val.replace(/[^0-9\.]/g, '')
    else if (key.includes('qty'))
      val = formatInput2(val)
    
    obj = this.setter(key, val, obj)
    this.handleSend(this, obj)
    states.cells[no] = obj
    this.props.setStates({states: obj})
  }
  
  handleClick() {
    var {name, no, key, states, obj} = this.initData(this, event)
    var val = this.props.data1
    
    // 1.0 差價
    var qtySpread = 10000
    var ratioSpread = 2
    
    if (key.includes('wnt'))
      var priceSpread = getSpread(val)
    else if (key.includes('stock') && isETF(obj.stock.code.code) == false)
      var priceSpread = getSpreadStock(val)
    else if (key.includes('stock') && isETF(obj.stock.code.code) == true)
      var priceSpread = getSpreadETF(val)
    
    if (priceSpread==0 && key.includes('wnt'))
      priceSpread = 0.001
    else if (priceSpread==0 && key.includes('stock'))
      priceSpread = 0.01

    // 2.0 初始化
    if (!val || typeof val == 'undefined' || val === null) {
      if (key.includes('price')) val = 0.001
      else if (key.includes('qty')) val = formatInputUnit(qtySpread, false)
      else if (key.includes('ratio')) val = 2
    }
    
    // 3.0 價
    else if (key.includes('price') && name == 'add') {
      val += priceSpread
      val = parseFloat(parseFloat(val).toFixed(3))
    }
    else if (key.includes('price') && name == 'subtract') {
      if (val-priceSpread > 0) val-=priceSpread
      val = parseFloat(parseFloat(val).toFixed(3))
    }
    
    // 4.0 量
    else if (key.includes('qty') && name == 'add') {
      val = formatInputUnit(val, true)
      val += qtySpread
      val = formatInputUnit(val, false)
    }
    else if (key.includes('qty') && name == 'subtract') {
      val = formatInputUnit(val, true)
      if (val-qtySpread > 0) val-=qtySpread
      val = formatInputUnit(val, false)
    }
    
    // 5.0 量比
    else if (key.includes('ratio') && name == 'add') {
      val += ratioSpread
    }
    else if (key.includes('ratio') && name == 'subtract') {
      if (val-ratioSpread > 0) val-=ratioSpread
    }
    
    // 6.0 更新狀態
    obj = this.setter(key, val, obj)
    this.handleSend(this, obj)
    states.cells[no] = obj
    this.props.setStates({states: obj})
  }
  
  // 前置攔置器
  initData(that, event) {
    return {
      name: event.target.name,
      val: formatInput(event.target.value).toLowerCase(),
      no: that.props.no,
      key: that._reactInternalFiber.key,
      states: that.props.getStates(),
      userId: parseInt(that.props.getStates().userId),
      algoName: that.props.getStates().modules.call,
      obj: $.extend(true, {}, that.props.getStates().cells[that.props.no])
    }
  }
  
  //
  setter(k, val, obj) {
    if (k == 'wnt.buy.price')
      obj.wnt.buy.price = val
    else if (k == 'wnt.buy.qty')
      obj.wnt.buy.qty = val
    else if (k == 'wnt.sell.price')
      obj.wnt.sell.price = val
    else if (k == 'wnt.sell.qty')
      obj.wnt.sell.qty = val
    else if (k == 'wnt.stopLoss.price')
      obj.wnt.stopLoss.price = val
    
    else if (k == 'stock.buy.price')
      obj.stock.buy.price = val
    else if (k == 'stock.buy.qty')
      obj.stock.buy.qty = val
    else if (k== 'stock.buy.ratio')
      obj.stock.buy.ratio = val
    else if (k == 'stock.sell.price')
      obj.stock.sell.price = val
    else if (k == 'stock.sell.qty')
      obj.stock.sell.qty = val
    else if (k== 'stock.sell.ratio')
      obj.stock.sell.ratio = val
    
    return obj
  }
  
  render() {
    var data1 = this.props.data1
    if (this.props.data1 === null || typeof this.props.data1 == 'undefined') data1 = ''
    
    return(
      <React.Fragment>
      
      <div className="input-group input-group-ticket">
      <input type="text" className="form-control" value={data1} onChange={this.handleChange} disabled={this.props.data2} />
      <div className="input-group-append">
        <button className="btn btn-secondary" type="button" name="subtract" onClick={this.handleClick} disabled={this.props.data2} >-</button>
        <button className="btn btn-secondary" type="button" name="add" onClick={this.handleClick} disabled={this.props.data2} >+</button>
      </div>
      </div>
      
      </React.Fragment>)
  }
}