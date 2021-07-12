class Recommender extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {underlying: 'HSI', issuer: 'VT', type: 'RC'}
    this.handleChange = this.handleChange.bind(this)
    this.handleClick = this.handleClick.bind(this)
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
    var url = 'http://192.168.21.119:8080/realtime?action=spread&underlying='+this.state.underlying+'&issuer='+this.state.issuer+'&type='+this.state.type+'&sort=tt&spread=1&format=json'
    var data = [{"Code":62605,"Type":"RP","Name":"VT#HSI RP2110O","Bid":0.106,"Ask":0.107,"BidIssuerSize":25150000,"AskIssuerSize":24850000,"Cratio":12500,"LotSize":10000,"Spread":0.001},{"Code":57164,"Type":"RP","Name":"VT#HSI RP2109C","Bid":0.188,"Ask":0.19,"BidIssuerSize":30000000,"AskIssuerSize":30000000,"Cratio":12500,"LotSize":10000,"Spread":0.001},{"Code":61217,"Type":"RP","Name":"VT#HSI RP2110L","Bid":0.117,"Ask":0.12,"BidIssuerSize":30000000,"AskIssuerSize":30000000,"Cratio":12500,"LotSize":10000,"Spread":0.001},{"Code":60405,"Type":"RP","Name":"VT#HSI RP2110F","Bid":0.174,"Ask":0.176,"BidIssuerSize":30000000,"AskIssuerSize":30000000,"Cratio":12500,"LotSize":10000,"Spread":0.001},{"Code":60765,"Type":"RP","Name":"VT#HSI RP2110I","Bid":0.163,"Ask":0.153,"BidIssuerSize":20040000,"AskIssuerSize":26300000,"Cratio":12500,"LotSize":10000,"Spread":0.001},{"Code":69051,"Type":"RP","Name":"VT#HSI RP2109T","Bid":0.213,"Ask":0.216,"BidIssuerSize":30000000,"AskIssuerSize":30000000,"Cratio":12500,"LotSize":10000,"Spread":0.001},{"Code":52559,"Type":"RP","Name":"VT#HSI RP2109J","Bid":0.192,"Ask":0.193,"BidIssuerSize":16000000,"AskIssuerSize":16000000,"Cratio":20000,"LotSize":10000,"Spread":0.001},{"Code":66143,"Type":"RP","Name":"VT#HSI RP2108N","Bid":0.25,"Ask":0.255,"BidIssuerSize":20000000,"AskIssuerSize":20000000,"Cratio":12500,"LotSize":10000,"Spread":0.001}]
    
  }
  
  getText(lang) {
    var text = {
      en: {bear: 'Bear', bull: 'Bull', submit: 'Submit', code: 'Code', type: 'Type', name: 'Name', bid: 'Bid', ask: 'Ask', bidIssuerSize: 'Bid Issuer Size',  askIssuerSize: 'Ask Issuer Size', cratio: 'Cratio'},
      sc: {bear: '熊证', bull: '牛证', submit: '提交'},
      tc: {bear: '熊證', bull: '牛證', submit: '提交'}
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
    <col span="1" width="100px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
    <col span="1" width="150px" />
  </colgroup>
  <thead>
    <tr>
    <th>{text.code}</th>
    <th>{text.type}</th>
    <th>{text.name}</th>
    <th>{text.bid}</th>
    <th>{text.ask}</th>
    <th>{text.bidIssuerSize}</th>
    <th>{text.askIssuerSize}</th>
    <th>{text.cratio}</th>
    <th></th>
    </tr>
  </thead>
  <tbody>
  </tbody>
  </table>
</div>

</div>
    )
  }
}