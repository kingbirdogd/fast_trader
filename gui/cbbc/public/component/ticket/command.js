class Command extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
    this.state.command = '{"cmd":"get_pricetable", "code": 16000, "algo_name":"kenny_semipro"}'
    this.handleClick = this.handleClick.bind(this)
    this.handleChange = this.handleChange.bind(this)
  }
  
  componentDidMount() {
  }
  
  handleChange() {
    this.setState({command: event.target.value.replaceAll(' ', '').replaceAll('"', "'")})
  }
  
  handleClick() {
    sendWebsocket(this.state.command.replaceAll("'", '"'))
  }
  
  render() {
    return(
      <React.Fragment>

<div className="row mb-2 mb-sm-2">
<div className="col-12 col-sm-12">

<form>
<div className="form-group">
<span style={{fontSize: '12px', color: 'red'}}>测试中</span>
<input style={{width: '100%', fontSize: '12px'}} type="text" id="tvCommand" className="form-control" onChange={this.handleChange} value={this.state.command} />
<button style={{fontSize: '11px'}} type="button" name="send" className="btn btn-sm btn-secondary" onClick={this.handleClick}>传送</button>
</div>
</form>

</div>
</div>

      </React.Fragment>
    )
  }
}