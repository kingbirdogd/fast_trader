class Recommender extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {underlying: '1', issuer: 'BI', type: 'EC', products: []}
    this.handleChange = this.handleChange.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.handleSelect = this.handleSelect.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleChange(event) {
    var {name, value} = event.target
    
    if (name == 'underlying')
      this.setState({'underlying': value})
    else if (name == 'issuer')
      this.setState({'issuer': value})
    else if (name == 'type')
      this.setState({'type': value})
  }
  
  handleClick(event) {
    async function getData(that) {
      var url = 'https://chart.dbpower.com.hk/buysellchart/cbbc_router.php?action=spread&underlying='+that.state.underlying+'&issuer='+that.state.issuer+'&type='+that.state.type+'&sort=tt&spread=1&format=json'
      var data = await $.ajax({url: "./cross-data", type: "post", data:{url: url}})
      if (data != '')
        that.setState({products: JSON.parse(data)})
    }
    getData(this)
  }
  
  handleSelect(event) {
    event.preventDefault()
    var curCode = event.target.getAttribute('data-code')
    
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states)
    var obj2 = $.extend(true, {}, states.cells)
    
    if ('modules' in obj && ('bear' in obj.modules || 'bull' in obj.modules)) {
      // get product
      var curProduct = null
      for (var i in this.state.products) {
        if (this.state.products[i].Code == curCode)
          curProduct = this.state.products[i]
      }

      // only cbbc
      if ('Type' in curProduct && (curProduct.Type == 'RC' || curProduct.Type == 'RP')) {
        for (var i in obj.config.value) {
          if (((obj.config.value[i] == 'bull' && curProduct.Type == 'RC')         // fill bull
                || (obj.config.value[i] == 'bear' && curProduct.Type == 'RP'))    // fill bear
              && obj2[i].action.code.value == '') {                               // not empty
              
            if (curProduct.AskIssuerSize<=0)
              curProduct.AskIssuerSize=0
            
            obj2[i].action.code.value = curProduct.Code.toString()
            obj2[i].action.issuerSize.value = formatInputUnit(curProduct.AskIssuerSize*0.8, false)
            obj2[i].action.quantity.value = formatInputUnit(10000, false)
            obj2[i].action.spread.value = curProduct.Spread.toString()
            obj2[i].action.delta.value = '0'
            
            if (curProduct.Type == 'RC')
              var algoName = obj.modules.bull
            else if (curProduct.Type == 'RP')
              var algoName = obj.modules.bear
            
            var command1 = {algo_name: algoName, cmd: 'get_warrant_detail', code: parseInt(curProduct.Code), id: states.userId, no: parseInt(i)}
            sendWebsocket(JSON.stringify(command1))
            
            break 
          }
        }
        
      }
    }
    
    this.props.setStates({'cells': obj2})
  }
  
  getText(lang) {
    var text = {
      en: {ucode: 'Underlying', bear: 'Bear', bull: 'Bull', call: 'Call', put: 'Put', submit: 'Submit', code: 'Code', type: 'Type', name: 'Name', bid: 'Bid', ask: 'Ask', bidIssuerSize: 'Bid Issuer Size',  askIssuerSize: 'Ask Issuer Size', spread: 'Spread', cratio: 'Cratio', select: 'Select', tickCount: 'Tick Count', lastTradeTime: 'Last Trade Time'},
      sc: {ucode: '标的', bear: '熊证', bull: '牛证', call: '认购', put: '认沽', submit: '提交', code: '编号', type: '种类', name: '名称', bid: '买入价', ask: '卖出价', bidIssuerSize: '买入量',  askIssuerSize: '卖出量', spread: '格数', cratio: '对冲值', select: '选择', tickCount: '小单/中单/大单', lastTradeTime: '最後交易时间'},
      tc: {ucode: '正股', bear: '熊證', bull: '牛證', call: '認購', put: '認沽', submit: '提交', code: '編號', type: '種類', name: '名稱', bid: '買入價', ask: '賣出價', bidIssuerSize: '買入量',  askIssuerSize: '賣出量', spread: '格數', cratio: '對沖值', select: '選擇', tickCount: '小單/中單/大單', lastTradeTime: '最後交易時間'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    
    var issuerHTML = []
    for (const [k, v] of Object.entries(this.props.data)) {
      issuerHTML.push(
        <option value={k} key={'issuer_'+k}>
          {k} ({v[this.props.lang]})
        </option>)
    }
    
    var productsHTML = []
    for (var i in this.state.products) {
      var p = this.state.products[i]
      
      // OMDD code
      var ucode = null
      if (parseInt(p.UCode) == 4001) ucode = 'HSI 恒生指数'
      else if (parseInt(p.UCode) == 4002) ucode = 'HSCE 国企指数'
      else if (parseInt(p.UCode) == 4038) ucode = 'HSTEC 恒生科技指数'
      else ucode = p.UCode+' '+getUnderlyingName2(p.UCode)
      
      productsHTML.push(
        <tr key={'product_'+i}>
          <td><a href="" data-code={p.Code} onClick={this.handleSelect}> {p.Code} </a></td>
          <td>{ucode}</td>
          <td>{p.Type}</td>
          <td>{p.Name}</td>
          <td>{p.Bid}</td>
          <td>{p.Ask}</td>
          <td>{formatInputUnit(p.BidIssuerSize, false)}</td>
          <td>{formatInputUnit(p.AskIssuerSize, false)}</td>
          <td>{p.Spread}</td>
          <td>{p.Cratio}</td>
          <td>{p.CountTick.replaceAll(':', ' : ')}</td>
          <td>{moment(p.LastTradeTime,'hhmmss').format('hh:mm:ss')}</td>
        </tr>
      )
    }
    
    if (this.state.products.length == 0) {
      productsHTML.push(
        <tr key={'product_0'}>
          <td colSpan="12" className="text-center">No available products</td>
        </tr>
      )
    }
    
    return(
<div className='row'>
<div className="col-12 col-sm-12 col-md-12 mb-2">

<div className="form-row form-recommend">
  <div className="form-group col-3 col-sm-3 col-md-2 mb-2">
    <input type="text" name="underlying" className="form-control" onChange={this.handleChange} defaultValue={this.state.underlying} />
  </div>
  
  <div className="form-group col-4 col-sm-3 col-md-2 mb-2">
    <select name="issuer" className="form-control" onChange={this.handleChange}> {issuerHTML} </select>
  </div>
  
  <div className="form-group col-3 col-sm-3 col-md-2 mb-2">
    <select name="type" className="form-control" onChange={this.handleChange}>
    <option value="EC">{text.call}</option>
    <option value="EP">{text.put}</option>
    <option value="RC">{text.bull}</option>
    <option value="RP">{text.bear}</option>
    </select>
  </div>
  
  <button name="send" type="button" className="btn btn-sm btn-secondary mb-2" onClick={this.handleClick}>{text.submit}</button>
</div>
</div>

<div className="col-12 col-sm-12 col-md-12 mb-2">
  <table className="table table-sm table-striped table-light table-recommand">
  <colgroup>
    <col span="1" width="50px" />
    <col span="1" width="150px" />
    <col span="1" width="80px" />
    <col span="1" width="150px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="100px" />
  </colgroup>
  <thead>
    <tr>
    <th>{text.code}</th>
    <th>{text.ucode}</th>
    <th>{text.type}</th>
    <th>{text.name}</th>
    <th>{text.bid}</th>
    <th>{text.ask}</th>
    <th>{text.bidIssuerSize}</th>
    <th>{text.askIssuerSize}</th>
    <th>{text.spread}</th>
    <th>{text.cratio}</th>
    <th>{text.tickCount}</th>
    <th>{text.lastTradeTime}</th>
    </tr>
  </thead>
  <tbody>
  {productsHTML}
  </tbody>
  </table>
</div>

</div>
    )
  }
}